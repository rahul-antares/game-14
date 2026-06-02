#!/bin/env node

const { request } = require('undici');
const beautify    = require("json-beautify");
const { program } = require('commander');
const colors      = require('colors');

function myParseInt(value, dummyPrevious) {
    const parsedValue = parseInt(value, 10);
    if (isNaN(parsedValue)) {
        throw new commander.InvalidArgumentError('Not a number.');
    }
    return parsedValue;
}

program
    .showHelpAfterError()
    .requiredOption('-g, --game-id <gameId>', 'game id')
    .requiredOption('--nh <nats host>', 'nats host', 'localhost')
    .option('-m, --currencyMultiplier <currency multiplier>', 'currency exchange value or currency multiplier, default is 1', myParseInt, 1)
    .option('-b --burst-size <burst size>', 'burst size', myParseInt, 1000)
    .option('-i --iterations <iterations>', 'iterations', myParseInt, 10)
    .parse (process.argv)
;
const options = program.opts ();
const natsOpts = {
    servers: `${options.nh || 'localhost'}:4222`
};

const { burstSize, iterations } = options;

async function burst(iteration) {
    const all          = [];
    const method       = 'play';
    const betAmount    = 10;
    const refBetAmount = 10;
    const featureMode  = 0;
    const featureBuy   = false;
    const start        = new Date();

    for (let i = 0; i < burstSize; i++) {
        const promise = request(`http://localhost:8080/${method}?betAmount=${betAmount}&refBetAmount=${refBetAmount}&featureMode=${featureMode}&featureBuy=${featureBuy}`);
        all.push (promise);
    }

    console.log (`[iteration ${iteration}] sent ${burstSize} play requests ...`);

    await Promise.allSettled (all)
        .then ((results) => {
            const duration = (new Date()).valueOf() - start.valueOf();
            let failed = 0;

            for (let i = 0; i < burstSize; i++) {
                if (results[i].status === 'rejected')
                    failed++;
            }

            console.log (`burst result:`);
            console.log (`  failure rate: ${(failed*100/burstSize).toFixed(2)}%%`);
            console.log (`  responses/s : ${((burstSize - failed)*1000/duration).toFixed(2)}`);
        })
    ;
}

async function main () {
    const payload = {
        method       : 'play',
        betAmount    : 10,
        refBetAmount : 10,
        featureMode  : 0,
        currencyMultiplier : options.currencyMultiplier,
        featureBuy   : false,
    };

    for (i = 0; i < iterations; i++)
        await burst(i);
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
