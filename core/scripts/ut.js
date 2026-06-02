#!/bin/env node

const os                      = require('os');
const fs                      = require('fs');
const path                    = require('path');
const colors                  = require('colors');
const { checkNegativeValues } = require('./overflow');
const sprintf                 = require('sprintf-js').sprintf;
const { program }             = require('commander');
const beautify                = require("json-beautify");

function round(num, decimalPlaces = 0) {
    const p = Math.pow(10, decimalPlaces);
    const n = (num * p) * (1 + Number.EPSILON);
    return Math.round(n) / p;
}

function myParseInt(value) {
    const parsedValue = parseInt(value, 10);
    if (isNaN(parsedValue)) {
        throw new program.InvalidArgumentError('Not a number.');
    }
    return parsedValue;
}

program
    .showHelpAfterError()
    .option('-b, --betAmount <amount>', 'bet amount', parseFloat)
    .option('-e, --extraSpin <bool>', 'extra-spin play', false)
    .option('-y, --buyFeature', 'buy feature', false)
    .option('-m, --currencyMultiplier <value>', 'currency multiplier value, default value is 1', myParseInt, 1)
    .option('-f, --featureMode <mode>', 'feature mode', parseFloat, 0)
    .option('-g, --gamble <gamble>', 'gamble(g) or (collect)c', 'c')
    .option('-i, --include <module...>', 'relative path of UT modules to include (can specify multiple options)')
    .option('-x, --exclude <module...>', 'relative path of UT modules to exclude (can specify multiple options)')
    .option('-v, --variant <variant>', '96 or 94', '96')
    .option('-V, --verbose', 'verbose')
    .option('-l, --list-modules', 'list modules')
    .option('-q, --quiet', 'show only errors') .option('--raw', 'dump raw JSON for post processing', false)
;

program.parse (process.argv);

const utModulesDir = path.join(__dirname, 'modules');
const options = program.opts ();

const indexFile   = `../index${ options.variant }`;
process.stderr.write (colors.green(`loading ${indexFile}\n`))

const { GameWrapper } = require(indexFile);

if (options.listModules) {

    //
    // Just list the modules and exit
    //

    let fpath;
    const files = fs.readdirSync (utModulesDir);

    try {
        for (let file of files) {
            if (!file.match(/\.js$/))
                continue;
            fpath = path.join (utModulesDir, file);
            const t = require (fpath);
            const f = file.replace (/\.js$/, '');
            console.log (sprintf('module %\'.32s ( %40s ) @ %s',
                colors.green(f), colors.yellow(t.name), fpath
            ));
        }
    }
    catch (err) {
        console.error (colors.red ('Error:'), 'modules =', fpath);
        console.error (err);
    }

    process.exit (0);
}

if (!options.betAmount) {
    program.outputHelp();
    process.exit(0);
}

const playParams = {
    betAmount          : options.betAmount,
    buyFeature         : options.buyFeature,
    featureMode        : options.featureMode,
    currencyMultiplier : options.currencyMultiplier || 1, // this causes modification in betAmount
    options: {
        extraSpin: false,
    }
};

const tests = [];

function matchPath (pathArr, file) {
    return pathArr.find ((p) => {
        const regExp = new RegExp (`${file}$`)
        if (p.match (regExp))
            return true;
        return false;
    });
}

function loadTests () {
    console.log ('searching directory', colors.yellow(utModulesDir), 'for modules ...');

    const files = fs.readdirSync (utModulesDir);
    for (let file of files) {
        try {
            if (!file.match (/\.js$/))
                continue;

            if (options.include?.length) {
                if (!matchPath (options.include, file))
                    continue
            }

            if (options.exclude?.length) {
                if (matchPath (options.exclude, file))
                    continue
            }

            const fpath = path.join (utModulesDir, file);
            const t = require (fpath);
            tests.push ({ ...t, file: fpath });
            console.log (`      loaded unit ${colors.yellow (file)} [ "${colors.magenta(t.name)}" ] ...`);
        }
        catch (err) {
            console.error (colors.red ('Error loading module'), colors.yellow(file));
            throw err;
        }
    }
}

function play (playParams) {
    let playResponse;
    let stage = 'play';

    try {
        playResponse = GameWrapper.play (playParams);

        while (!playResponse.ended) {
            const nextPlayOpts = {};
            const { fsStatus } = playResponse;

            if (fsStatus && fsStatus.length > 0 && fsStatus[fsStatus.length-1].status === "INIT") {
                const gamble = options.gamble.trim() == 'g';
                nextPlayOpts['action'] = gamble ? 'gamble' : 'collect';
                nextPlayOpts['gamble'] = gamble;
                playResponse = GameWrapper.next (nextPlayOpts, playResponse);
            }
        }

        // if (options.extraSpin) {
        //     while (playResponse.ended && playResponse.buyExtraSpin.cost > 0) {
        //         const { buyExtraSpin } = playResponse;
        //         const esPlayParams = {
        //             betAmount          : buyExtraSpin.expectedBet,
        //             buyFeature         : true,
        //             featureMode        : buyExtraSpin.featureMode,
        //             currencyMultiplier : options.currencyMultiplier || 1,
        //             options: {
        //                 extraSpin: true,
        //                 prevState: playResponse,
        //             }
        //         };
        //
        //         playResponse = GameWrapper.play (esPlayParams);
        //     }
        // }
    } catch (err) {
        console.error (colors.red ('GameWrapper error:'), `failed at state "${stage}"`);
        console.error (beautify(playResponse, null, 2, 100));
        throw err;
    }

    return playResponse;
}

const levels = {
    count: [0, 0, 0, 0, 0],
    wins:  [0, 0, 0, 0, 0],
    rounds : 0,
    winsTotal: 0,
};

function displayResultsFSLvl () {
    const betAmt = (levels.rounds * 700);

    console.log ("Frequency");
    for (let i = 1; i < 5; i++)
        console.log (`Level ${i}: 1 in ${round(levels.rounds / levels.count[i], 3)}`);

    console.log ("\nRTP");
    for (let i = 1; i < 5; i++)
        console.log (`Level ${i}: ${round(levels.wins[i] / betAmt, 5)}`);

    console.log (`Total: ${round(levels.winsTotal / betAmt, 5)}`);
}

function rtpByLevelsFS (response) {
    const { freeSpins } = response;
    if (!freeSpins || freeSpins.length === 0)
        return;

    let prevLvl = freeSpins[0].level;

    levels.count[prevLvl] += 1;

    for (const fs of freeSpins) {
        if (fs.level > prevLvl) {
            prevLvl = fs.level;
            levels.count[prevLvl] += 1;
        }

        levels.wins[fs.level] += fs.winAmount;
        levels.winsTotal += fs.winAmount;
    }

    levels.rounds += 1;
}

function runTests (config, playParams, response, count) {
    for (let i = 0; i < tests.length; i++) {
        const { name, file, func } = tests[i];

        if (options.verbose) console.log (`running test ${colors.yellow(name)} (file ${file}) ...`);

        const resultStr = func (config, playParams, response);

        if (resultStr) {
            response.internal = {};
            console.error (beautify(response, null, 2, 100));
            console.error (colors.yellow (name), colors.bgRed ('Failed:'), colors.reset (resultStr));
            console.error (`(test module => ${colors.yellow (file)})`)
            throw new Error ('test failed')
        }
        else if (!options.quiet) {
            console.log ('test', colors.yellow (name), colors.green('passed'));
        }
    }
}

function printFsTable(fsBy, count) {
    console.table({ ...fsBy, rounds: count });

    console.log ("% FS Contribution :");
    console.log ("Normal =", (fsBy.normal/fsBy.total).toFixed(5));
    console.log ("Re-Spin =", (fsBy.reSpin/fsBy.total).toFixed(5));
    console.log ("Scatter Hook =", (fsBy.scatterHook/fsBy.total).toFixed(5));
    console.log ("FS hit = 1 in", (count / fsBy.total).toFixed(5));
    console.log ();
}

let count = 0;
async function main () {
    let response;

    console.error ('using RNG API', colors.yellow(GameWrapper.getRNGLibType().type));

    loadTests ();

    GameWrapper.initRNG({
        nats: {
            url : 'nats://localhost:4222',
            rngCh: 'rng.binary.client',
            timeout : 2000,
            name: 'game5',
            verbose : false,
            allowReconnect : true,
            retryOnFailedConnect : true,
            maxReconnect : 50,
            reconnectWait : 500,
            pingInterval : 5000,
            maxPingsOut : 5,
        },
        grpc: {
            host: 'localhost',
            port: 50051
        }
    });

    let scatterHookCount = 0;
    const config = GameWrapper.getConfig();

    const fsBy  = {
        normal: 0,
        reSpin: 0,
        scatterHook: 0,
        total : 0,
    };

    while (true) {
        response = play (playParams);
        if (response.baseSpin.scatterHook)
            scatterHookCount++;

        // if (response.fsStatus.length > 0) {
        //     const { baseSpin } = response;
        //     if (baseSpin.scatterHook === true) {
        //         fsBy.scatterHook += 1;
        //     } else if (baseSpin.numSC === 2 && baseSpin.reSpins.length > 0) {
        //         fsBy.reSpin += 1;
        //     } else if (baseSpin.numSC === 3 && baseSpin.reSpins.length === 0 && baseSpin.scatterHook === false) {
        //         fsBy.normal += 1;
        //     }

        //     fsBy.total += 1;
        // }

        checkNegativeValues (response);
        runTests (config, playParams, response, count);

        rtpByLevelsFS(response);

        count++;

        if (!(count % 100000)) {
            const memTotal = parseFloat((os.totalmem() / 1024 / 1024).toFixed(2)).toLocaleString() + ' MB';
            const memFree  = parseFloat((os.freemem()  / 1024 / 1024).toFixed(2)).toLocaleString() + ' MB';
            console.log (`+ ${count.toLocaleString()} iterations (mem = ${colors.yellow(memFree)} / ${memTotal})`);

            // console.log ("Scatter Hook Prob =", parseFloat(scatterHookCount/count).toFixed(5).toLocaleString());
            // printFsTable (fsBy, count);
            console.table (levels);
            displayResultsFSLvl();
        }

    }
}

main ()
    .then (() => {
        process.exit (0);
    })
    .catch ((err) => {
        console.error (`(Iteration #${count.toLocaleString()})`, colors.red ('Error:'), err);
        process.exit (1);
    })
;
