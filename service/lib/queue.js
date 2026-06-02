const EventEmtter = require('eventemitter2');
const ThreadCtrl  = require('./thread-ctrl.js');

const PERIOD = 100;

class Monitor extends EventEmtter {
    constructor (q) {
        super ();
        this.q      = q;
        this.maxAge = q.maxAge;
        const _this = this;

        this.timer = setInterval (() => {
            _this._checkQ ();
        }, PERIOD);
    }

    deinit () {
        clearInterval (this.timer);
        this._checkQ (true);
    }

    _checkQ (force = false) {
        const now = (new Date ()).valueOf();

        for (let i = 0; i < this.q.length; i++) {
            const ts = this.q[i].ts.valueOf();

            if (force || (now - ts > this.maxAge)) {
                const el = this.q.dequeue();
                el.reject ('sla violated');
            }
        }
    }
}

class Q {
    #in = []; //TODO: use a faster queue
    monitor;

    constructor ({ maxAge = 500 }) {
        this.maxAge = maxAge;

        if (!this.monitor)
            this.monitor = new Monitor (this);
    }

    deinit () {
        this.monitor.deinit ();
    }

    enqueue (m) {
        this.#in.push (m);

        return m.promise;
    }

    dequeue () {
        return this.#in.pop ();
    }

    get _isEmpty () {
        return this.#in.length === 0;
    }
}

module.exports = Q;
