/*
 * Uncomment this line if GRPC RNG is used
 *     process.env['GRPC_DNS_RESOLVER'] = 'native';
 */

const { Game } = require('./build/lib/game96.node'); 
const game = new Game();

class GameWrapper {
	static getConfig(options) {
        const config = game.getConfig(options);

        // NOTE: After game 1 certification, gameStudio has been renamed to studio,
        // Game 4 had gameStudio key and had been certified, so following change is for the same
        // and not done in core, BUT only for this game.
        if (config.gameStudio && !config.studio) {
            config.studio = config.gameStudio;
        }

        return config;
	}

	static getRNGLibType() {
		return game.getRNGLibType(); 
	}

    static initRNG(opts) {
        return game.initRNG (opts);
    }

    static init() {
        return game.init ();
    }

    static play(playOptions) {
        return game.play (playOptions); 
	}

	static next(nextOpts, playResponse) {
		return game.next (nextOpts, playResponse); 
	}

    static reseed() {
        return game.reseed ();
    }
}

module.exports = { GameWrapper }; 
