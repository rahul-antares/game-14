const colors                   = require ('colors');
const {
    Worker, MessageChannel,
    MessagePort, isMainThread,
    parentPort,
}                              = require ('node:worker_threads');
const Q                        = require ('./queue');
const Nats                     = require ('../common/nats-core');
const Config                   = require ('../config');
const log                      = require ('../common/log').child ({ module: 'threads' });

const INIT  = 'init';
const IDLE  = 'idle';
const BUSY  = 'busy';
const ERROR = 'error';

class Threads {
    static workers = {};
    static q;
    static processing = {};
    static endResolve;
    static timer;
    static instanceId;

    static start ({ numThreads, instanceId, gameEntryIndexFile }) {
        if (!numThreads || !instanceId)
            throw new Error ('insufficient args');

        this.instanceId = instanceId;
        this.gameEntryIndexFile = gameEntryIndexFile;
        this.q = new Q ({ maxAge: Config().game.reqMaxAge });

        for (let i = 0; i < numThreads; i++)
            this._startThread ();

        // set periodic rng sequence monitor
        this.timer = setInterval (() => {
            this.xmitAndClearRHistory ();
        }, Config().rng.maxHoldTime)
    }

    static _startThread () {
        const worker = new Worker ('./lib/game.js', {
            workerData: { gameEntryIndexFile: this.gameEntryIndexFile },
        });

        const id         = worker.threadId;
        this.workers[id] = {
            worker,
            hiPrioQ     : new Q ({ maxAge: Config().game.reqMaxAge }),
            randRecords : new Buffer.alloc(0),
            state       : INIT
        };

        worker.on ('message', (msg) => {
            this.handleMsg (worker, msg);
        });

        worker.on ('exit', (code) => {
            log.warn ({ threadId: id, code }, 'thread exited');
            this._postThreadExitCleanup (worker.threadId);
        });
    }

    static async _stopThreadAndStartNew (id) {
        await this._forceStopThread (id);
        this._startThread ();
    }

    static async _forceStopThread (id) {
        const wData = this.workers[id];

        if (!wData)
            return;

        const exitCode = await wData.worker.terminate ();

        /*
         * no need to call this here. Should get called 
         * once the "exit" event is triggered
         *
        this._postThreadExitCleanup (id);
         *
         */
    }

    static _postThreadExitCleanup (id) {
        const wData  = this.workers[id];

        if (!wData)
            return;

        // Reject all jobs enqueued for this thread
        Object.keys (this.processing).forEach ((seq) => {
            const { m, id:threadId } = this.processing[seq];
            if (id !== threadId)
                return;

            m.reject ('thread stopped');
            delete this.processing[seq];
        });

        this._xmitThreadRandRecords (id);

        delete this.workers[id];
    }

    static submit (m) {
        let promise;

        switch (m.id) {
            case 'reseed':
                const { threadId, instanceId } = m.data;

                if (!threadId || !instanceId)
                    throw new Error ('insufficient params for "reseed"');

                const wData = this.workers[threadId];

                if (!wData || this.instanceId !== instanceId)
                    throw new Error ('ignoring "reseed" since it is not for me');

                promise = wData.hiPrioQ.enqueue (m);
                break;

            default:
                promise = this.q.enqueue (m);
        }

        process.nextTick (() => this._process ());
        return promise;
    }

    static _process () {
        let id;

        for (let id in this.workers) {
            const wData  = this.workers[id];

            if (wData.state !== IDLE)
                continue;

            const m = wData.hiPrioQ.dequeue () || this.q.dequeue ();

            if (!m)
                continue;

            wData.state            = BUSY;
            this.processing[m.seq] = { m, id };

            wData.worker.postMessage (m.serialize ());
        }
    }

    static processRandRecords ({ rand, threadId }) {
        const gameId = Config().id;
        const wData  = this.workers[threadId];

        if (!wData)
            return;

        const r           = rand.map((curr) => Buffer.from(curr.bStream));
        const cumBuf      = Buffer.concat(r);
        wData.randRecords = Buffer.concat ([ wData.randRecords, cumBuf ]);

        if (wData.randRecords.length >= Config().rng.maxRecords)
            this.xmitAndClearRHistory ();
    }

    static xmitAndClearRHistory () {
        Object.keys (this.workers).forEach ((threadId) => {
            this._xmitThreadRandRecords (threadId);
        });
    }

    static _xmitThreadRandRecords (threadId) {
        const wData = this.workers[threadId];

        if (!wData || !wData.randRecords.length)
            return;

        Nats.send (Config().rng.monitorChannel, {
            channel    : Config().gameCh,
            threadId,
            gameId     : Config().id,
            instanceId : this.instanceId,
            bStream    : wData.randRecords.toString('base64'),
        });

        wData.randRecords = new Buffer.alloc(0);
    }

    static handleMsg (worker, msg) {
        const threadId = worker.threadId;
        const { seq, id, type, data, error } = msg;
        let _throw = false;

        try {
            switch (id) {
                case 'ready':
                    this.handleReady (worker, error);
                    break;

                case 'init':
                case 'config':
                case 'reseed':
                    this.handleGameResponse (worker, seq, data, error);
                    if (id === 'reseed')
                        log.warn ({ threadId:worker.threadId }, 'reseeded');
                    break;

                case 'play':
                case 'next':
                    this.handleGameResponse (worker, seq, data, error);
                    if (!error)
                        this.processRandRecords({ rand:data.internal.rand, threadId });
                    break;

                default:
                    _throw = true;
                    const { m } = this.processing[seq];

                    if (m)
                        m.reject (error || `RX unknown message id "${id}" from worker thread`);
            }

            if (_throw)
                throw new Error (`RX unknown message id "${id}" from worker thread`)
        }
        catch (err) {
            log.error ({ err, m:msg }, 'handle message failed');
        }

        if (this.workers[threadId])
            this.workers[threadId].state = IDLE;

        delete this.processing[seq];
        this._process ();
    }

    static handleReady (worker, error) {
        const state    = error ? ERROR: IDLE;
        const threadId = worker.threadId;
        const wData    = this.workers[threadId];

        if (!wData) {
            log.warn ({ threadId }, 'no thread data for ready thread');
            return this._process ();
        }

        wData.state = state;

        log[state === ERROR? 'error':'info'] ({ state, threadId }, 'thread state');
        this._process ();
    }

    static handleGameResponse (worker, seq, data, error) {
        const { m } = this.processing[seq];

        if (!m)
            throw new Error (`unknown response`);

        if (!error)
            return m.resolve (data);

        // Check for RNG number generation failure and act

        if (error.indexOf('UNABLETOGENRANDOM') !== -1)
            process.nextTick (() => {
                this._stopThreadAndStartNew (worker.threadId);
            });

        m.reject (error);
    }
}

module.exports = Threads;
