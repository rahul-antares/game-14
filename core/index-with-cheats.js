const { Game } = require('./build/lib/game96-wc.node'); 
const game = new Game();

class GameWrapper {
	static getConfig(options) {
		return game.getConfig(options); 
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
		const cheats = playOptions.cheats;
		return game.play (playOptions, cheats); 
	}

	static next(nextOpts, playResponse) {
		return game.next (nextOpts, playResponse); 
	}

    static reseed() {
        return game.reseed ();
    }
}

module.exports = { GameWrapper }; 
