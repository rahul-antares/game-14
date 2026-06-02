#!/bin/env node

process.env['GRPC_DNS_RESOLVER'] = 'native';

const os                   = require('os');
const moment               = require('moment');
const momentDurFormatSetup = require('moment-duration-format');
const colors               = require('colors');
const sprintf              = require('sprintf-js').sprintf;
const beautify             = require("json-beautify");
const { program }          = require('commander');
const Threads              = require('./js/threads');

function simCommand () {
    const argStr = process.argv.slice(2).join(' ');
    return `./index.js ${argStr}`;
}

function printIntro (opts) {
    let color = colors.yellow;
    let sep = ':';

    if (opts.csvOutput) {
        color = (args) => args;
        sep   = ',';
    }

    console.log (` user     ${sep}`, color (os.userInfo().username));
    console.log (` ts       ${sep}`, color (new Date()));
    console.log (` hostname ${sep}`, color (os.hostname()));
    console.log (` machine  ${sep}`, color (os.machine()));
    console.log (` arch     ${sep}`, color (os.arch()));
    console.log (` cpus     ${sep}`, color (os.cpus().length));
    console.log (` platform ${sep}`, color (os.platform()));
    console.log (` command  ${sep}`, simCommand());
    console.log (` bet params :`, Object.keys(opts).reduce ((acc, key) => {
        if (opts.csvOutput) {
            acc += sprintf (`\n  ,${key}, ${opts[key]}`);
            return acc;
        }

        acc += sprintf (`\n    %-16s : %s`, key, colors.yellow (opts[key]));
        return acc;
    }, ''));
}

function myParseInt(value, dummyPrevious) {
    const parsedValue = parseInt(value, 10);
    if (isNaN(parsedValue)) {
        throw new commander.InvalidArgumentError('Not a number.');
    }
    return parsedValue;
}

program
    .showHelpAfterError()
    .option('-i, --iterations <num>', 'number of iterations', myParseInt)
    .option('-b, --betAmount <amount>', 'bet amount', parseFloat, 10)
	.option('-m, --currencyMultiplier <value>', 'currency multiplier value, default value is 1', myParseInt, 1)
    .option('-r, --refBetAmount <amount>', 'ref bet amount', parseFloat, 10)
    .option('-f, --featureMode <mode>', 'feature mode', parseFloat, 0)
    .option('-y, --buyFeature', 'buy feature', false)
    .option('-v, --variant <variant>', '96 or 94', '96')
    .option('-n, --next <string>', 'collect or gamble', "collect")
    .option('-B, --batch-size <num>', 'run batch size', myParseInt, 1_000_000)
    .option('--show-batch-data', 'show batch data (default shows cumulative data)', false)
    .option('-c, --csv-output', 'show csv formatted output(default shows normal statistical values)', false)
    .option('-t --threads <num>', 'num of threads to spawn', myParseInt, 0)
    .parse (process.argv)
;

const options = program.opts ();
momentDurFormatSetup (moment);

const iterations = options.iterations || Infinity;
const numThreads = options.threads || os.cpus().length - 1;
const playOpts   = {
    betAmount            : options.betAmount,
    featureMode          : options.featureMode,
    refBetAmount         : options.refBetAmount,
    buyFeature           : options.buyFeature ? 1 : 0,
    currencyMultiplier   : options.currencyMultiplier,
};

const sleep = ms => new Promise (r => setTimeout(r, ms));

async function main () {
    options.threads = numThreads;
    printIntro (options);
    Threads.start ({
        numThreads,
        iterations,
        playOpts,
        batchSize : options.batchSize,
        csvOutput : options.csvOutput,
        variant   : options.variant
    });
    await Threads.await ();
}

main()
    .then ((stats) => {
        process.exit (0);
    })
    .catch ((err) => {
        console.error (colors.red('ERR:'), err);
        console.error ('-- last error response --');
        console.error (beautify (harness.getLastErrorResponse(), null, 2, 100));
        process.exit(1);
    });

