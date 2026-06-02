const colors = require('colors');
const {
  Worker, MessageChannel, MessagePort, isMainThread, parentPort,
} = require('node:worker_threads');
const Data = require('./data');

const INIT = 'init';
const READY = 'ready';

class Threads {
    static workers = {};
    static iterations;
    static batchSize;
    static playOpts = {};
    static seq = 0;

    static start ({ numThreads, iterations, batchSize, playOpts, csvOutput, variant }) {
        if (!numThreads || !iterations || !playOpts)
            throw new Error ('insufficient args');

        Threads.playOpts   = playOpts;
        Threads.iterations = iterations;
        Threads.batchSize  = batchSize;
        Threads.csvOutput  = csvOutput;

        Data.endPromise = new Promise((resolve) => {
            Data.endResolve = resolve;
        });

        for (let i = 0; i < numThreads; i++) {
            const worker = new Worker ('./js/harness.js', {
                workerData: {
                    batchSize,
                    playOpts,
                    csvOutput,
                    variant,
                },
            });

            const id = worker.threadId;
            Threads.workers[id] = ({ worker, state: INIT });

            worker.on ('message', (msg) => {
                Threads.handleMsg (worker, msg, csvOutput);
            });

            worker.on ('exit', (code) => {
                delete Threads.workers[id];
                if (!Object.keys(Threads.workers).length) {
                    Data.endResolve();
                }
            });
        }
    }

    static async await () {
        await Data.endPromise;
    }

    static async end () {
        for (let id in Threads.workers) {
            await Threads.workers[id].worker.postMessage(Threads.createMsg(
                'stop'
            ));
        }

        await Threads.await ();
    }

    static handleMsg (worker, msg, csvOutput) {
        const { header, body } = msg;
        const { type } = header;

        switch (type) {
            case 'ready':
                return this.handleReady (worker, header, body);

            case 'data':
                return this.handleData (worker, header, body, csvOutput);
                break;

            default:
                console.error (colors.red (`ERR: unknown message type "${type}" RX from child thread`));
                console.error ('the full message is:', msg);
        }
    }

    static handleReady (worker, header, body) {
        console.error (`thread ${worker.threadId} is now ${colors.green('ready')}`);

        Threads.workers[worker.threadId].state = READY;
        Threads.workers[worker.threadId].worker.postMessage (Threads.createMsg(
            'start-batch', {
                batchSize: Threads.batchSize,
                playOpts:  Threads.playOpts
            },
        ));
    }

    static handleData (worker, header, body, csvOutput) {
        if (csvOutput)
            Data.showCsvOutput(body);
        else
            Data.showAccStats(body);
    }

    static createMsg (type, msg) {
        return {
            header: {
                seq: Threads.seq++,
                type: type,
            },
            body: msg,
        };
    }
}

module.exports = Threads;
