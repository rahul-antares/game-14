const { RngModule } = require('./build/lib/rng-module.node'); 

class RNG_Wrap {
    #rng

    constructor () {
        this.#rng = new RngModule ();
    }

	getVersion () {
		return this.#rng.getVersion ();
	}

	getScaledV (opts) {
		return this.#rng.getScaledV (opts);
	}

	getScaled (opts) {
		return this.#rng.getScaled (opts);
	}

	getScaledBinary (opts) {
		return this.#rng.getScaledBinary (opts);
	}
}

module.exports = RNG_Wrap;
