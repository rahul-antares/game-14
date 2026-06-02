const RNG = require('@core/lib');

class RNGController {
    #RNG

    constructor () {
        this.#RNG = new RNG ();
    }

    async init () {
    }

    getScaledBinary (data) {
        return this.#RNG.getScaledBinary (data);
    }

    getScaled (body) {
        const { data } = body;

        if (!Number.isInteger(data))
            throw new Error ('incorrect params: integer expected');

        return this.#RNG.getScaled (data);
    }

    getScaledV (body) {
        const { data } = body;

        if (!Array.isArray(data))
            throw new Error ('incorrect params: array expected');

        return this.#RNG.getScaledV (data);
    }
}

module.exports = RNGController;
