#!/bin/env node

const beautify    = require("json-beautify");
const { program } = require('commander');
const colors      = require('colors');
const NATS        = require('./nats-core');

program
    .showHelpAfterError()
    .requiredOption('-g, --game-id <gameId>', 'game id')
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
        method : 'config',
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
