const { Game } = require('./build/lib/game92-wc.node'); 
const game = new Game();

class GameWrapper {
    static getConfig(options) {
        const cheats = game.cheats();
        const config = game.getConfig(options);
        return { ...config, cheats }; 
    }

    static getRNGLibType() {
        return game.getRNGLibType(); 
    }

    static initRNG(opts) {
        return game.initRNG (opts);
    }

    static init(opts) {
        return game.init (opts);
    }

   static play(playOptions) {
        let response;
        const cheats = playOptions.cheats;
        response = game.play (playOptions, cheats);
        response.cheats = cheats;
        return response;
    }

    static next(nextOpts, playResponse) {
        const response = game.next(nextOpts, playResponse, playResponse.cheats);
        response.cheats = playResponse.cheats;
        return response;
    }

    static reseed() {
        return game.reseed ();
    }
}

module.exports = { GameWrapper }; 
