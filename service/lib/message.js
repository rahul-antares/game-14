let seq = 0;

const MTYPE = {
    REQUEST  : 0,
    RESPONSE : 1,
    INFO     : 2,
};

class Message {

    constructor (seq, type, id, data, ctx, error) {
        if (typeof seq === 'undefined')
            throw new Error ('"seq" is required');

        if ((type !== MTYPE.REQUEST) &&
            (type !== MTYPE.RESPONSE) &&
            (type !== MTYPE.INFO)
        )
            throw new Error ('invalid "type"');

        if (!id)
            throw new Error ('"id" is required');

        this.seq   = seq;
        this.type  = type;
        this.id    = id;
        this.data  = data;
        this.error = error;
        this.ctx   = ctx;
        this.ts    = new Date();
    }

    static makeRequest (id, data, ctx) {
        const m = new Message (++seq, MTYPE.REQUEST, id, data, ctx);
        m.promise = new Promise ((resolve, reject) => {
            m.resolve = resolve;
            m.reject  = reject;
        });

        return m;
    }

    static makeResponse (id, _seq, data, error) {
        return new Message (_seq, MTYPE.RESPONSE, id, data, null, error);
    }

    static makeInfo (id, data) {
        return new Message (++seq, MTYPE.INFO, id, data);
    }

    serialize () {
        return {
            seq   : this.seq,
            type  : this.type,
            id    : this.id,
            data  : this.data,
            error : this.error,
            ctx   : this.ctx,
            ts    : this.ts,
        };
    }
}

module.exports = Message;
