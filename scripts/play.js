#!/bin/env node

const beautify    = require("json-beautify");
const { program } = require('commander');
const colors      = require('colors');
const NATS        = require('./nats-core');

function myParseInt(value) {
    const parsedValue = parseInt(value, 10);
    if (isNaN(parsedValue)) {
        throw new program.InvalidArgumentError('Not a number.');
    }
    return parsedValue;
}


program
    .showHelpAfterError()
    .requiredOption('-g, --game-id <gameId>', 'game id')
    .option('-c --cheat', 'send a fixed (hardcoded cheat)')
    .option('-m, --currencyMultiplier <currency multiplier>', 'currency exchange value or currency multiplier, default is 1', myParseInt, 1)
    .option('--nh <nats host>', 'nats host', 'localhost')
    .parse (process.argv)
;
const options = program.opts ();
const natsOpts = {
    servers: `${options.nh || 'localhost'}:4222`
};

async function main () {
    await NATS.init(natsOpts);

    const payload = {
        method       : 'play',
        betAmount    : 10,
        refBetAmount : 10,
        featureMode  : 0,
        buyFeature   : false,
        currencyMultiplier : options.currencyMultiplier,
        cheats: {
            numSC: options.cheat ? 4 : -1,
        }
    };

    return await NATS.request(`ge.${options.gameId}`, { data: payload });
}

main ()
    .then ((r) => {
        console.log (colors.green ('Response OK'));
        console.log (beautify(r, null, 2, 100));
        process.exit (0);
    })
    .catch ((err) => {
        console.error (colors.red ('Error:'), err);
        process.exit (1);
    })
;
