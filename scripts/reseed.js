#!/bin/env node

const beautify    = require("json-beautify");
const { program } = require('commander');
const colors      = require('colors');
const NATS        = require('./nats-core');

program
    .showHelpAfterError()
    .requiredOption('-g, --game-id <gameId>', 'game id')
    .requiredOption('-t, --thread-id <thread id>')
    .requiredOption('-i, --instance-id <thread id>')
    .requiredOption('--nh <nats host>', 'nats host', 'localhost')
    .parse (process.argv)
;
const options = program.opts ();
const natsOpts = {
    servers: `${options.nh || 'localhost'}:4222`
};

async function main () {
    await NATS.init(natsOpts);

    const payload = {
        method     : 'reseed',
        threadId   : options.threadId,
        instanceId : options.instanceId,
    };

    NATS.send(`ge.${options.gameId}`, { data: payload });
}

main ()
    .then ((r) => {
        console.log (beautify(r, null, 2, 100));
        process.exit (0);
    })
    .catch ((err) => {
        console.error (colors.red ('Error:'), err);
        process.exit (1);
    })
;
