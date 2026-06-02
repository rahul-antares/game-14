#!/bin/env node

const colors                                      = require('colors');
const readline                                    = require('readline/promises');
const { stdin: input, stdout: output }            = require('process');
const util                                        = require('util');
const fs                                          = require('fs');
const path                                        = require('path');
const { checkNegativeValues }                     = require('./overflow');
const { colorize }                                = require('json-colorizer');
const { program }                                 = require('commander');
const beautify                                    = require('json-beautify');
const { isValidIndex, promptBoosterPick, prompt, rl } = require('./js/helper');
const { Parser }                                  = require('./js/parsers');

function myParseInt(value) {
    const parsedValue = parseInt(value, 10);
    if (isNaN(parsedValue)) {
        throw new program.InvalidArgumentError('Not a number.');
    }
    return parsedValue;
}



program
    .showHelpAfterError()
    .option('-c, --showConfigOnly', 'show config only', false)
    .option('-b, --betAmount <amount>', 'bet amount should be currencyMultiplier times', parseFloat)
    .option('-f, --featureMode <mode>', 'feature mode', parseFloat, 0)
    .option('-g, --gamble <gamble>', 'g(gamble) or c(collect)')
    .option('-e, --extraSpin <extraSpin>', 'y(yes) or n(no)')
    .option('-t, --truncate <value>', 'truncate some of not so imp arrays to show compact json', true)
    .option('-v, --variant <variant>', '96, 94, or 92', '96')
    .option('-y, --buyFeature', 'buy feature', false)
    .option('-m, --currencyMultiplier <value>', 'currency multiplier value, default value is 1', myParseInt, 1)
    .option('--raw', 'dump raw JSON for post processing', false)
    .option('-i --internal <bool>', 'show/not show "internal" data from output', false)
    .option('-q --quiet', 'quiet - just show the final response', false)
;

/* Add cheats */
const CheatsMap = [];

(function () {
    const variants    = ['96', '94', '92'];
    let cheatsList    = null;
    let loadedVariant = null;

    // this is needed since we need to show the help
    for (const v of variants) {
        const modulePath = path.resolve(__dirname, `../index${v}-wc.js`);

        if (!fs.existsSync(modulePath))
            continue;

        try {
            const wrapper = require(`../index${v}-wc`).GameWrapper;
            if (wrapper && wrapper.getConfig && wrapper.getConfig().cheats) {
                cheatsList = wrapper.getConfig().cheats;
                loadedVariant = v;
                break;
            }
        } catch (e) {
            continue; // skipping currently, no need to throw here
        }
    }

    if (!cheatsList) {
        console.error(colors.red("Error: No variant (96, 94, 92) with cheats found!"));
        // throw new Error("No cheat-enabled variant found.");
    }

    cheatsList && Object.keys(cheatsList).forEach((idx) => {
        const { name, implemented, description, type } = cheatsList[idx];
        const parser    = Parser[type.toLowerCase()];
        CheatsMap[name] = {implemented, name, parser, description };

        program.option(
            `--${name} <${type.toLowerCase()}>`,
            (!implemented ? colors.red('(NOT IMPLEMENTED) ') : '') + description,
            parser
        );
    });
})();

program.parse (process.argv);
const options = program.opts ();

/* Build the "cheats" object */
const cheats     = {};
const cheatNames = Object.keys (CheatsMap);
for (let argName in options) {
    if (cheatNames.indexOf(argName) >= 0)
        cheats[argName] = options[argName];
}


/* Check if any cheats are being used */
const usingCheats = Object.keys (cheats).length > 0;
const indexFile   = `../index${ options.variant }${usingCheats?'-wc':''}`;
process.stderr.write (colors.green(`loading ${indexFile}\n`))
const { GameWrapper } = require(indexFile);

if (options.showConfigOnly) {
    console.log (beautify (GameWrapper.getConfig(), null, 4, 10000));
    process.exit(0);
}

if (!options.betAmount) {
    program.outputHelp();
    process.exit(0);
}

const playParams = {
    betAmount          : options.betAmount,
    buyFeature         : options.buyFeature,
    featureMode        : options.featureMode,
    currencyMultiplier : options.currencyMultiplier || 1,
    options            : {
        extraSpin: false,
    },
};

function hasCheats(cheats) {
    return (Object.keys(cheats).find((curr) => typeof cheats[curr] !== 'undefined') !== null);
}

if (hasCheats(cheats))
    playParams.cheats = cheats;

async function main () {
    // console.error ('using RNG API', colors.yellow(GameWrapper.getRNGLibType().type));

    const status = GameWrapper.initRNG({
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

    let playResponse = GameWrapper.play (playParams);
    checkNegativeValues (playResponse);

    while (!playResponse.ended) {
        console.log (options.raw ?
            beautify (playResponse)
            : beautify(options.truncate ? getTruncatedResp(playResponse): playResponse, null, 2, 100)
        );

        const { fsStatus } = playResponse;
        const nextPlayOpts    = {};

        if (fsStatus.length > 0 && fsStatus[fsStatus.length-1].status === "INIT") {
            const gambleStr = (typeof options.gamble === 'undefined')? await prompt(): options.gamble;
            const gamble    = gambleStr.trim() == 'g';
            console.log (colors.yellow(`\nnext => ${gamble?"gamble":"collect"}\n`));
            nextPlayOpts['gamble'] = gamble;
            playResponse = GameWrapper.next (nextPlayOpts, playResponse);
        }
    }

    let extraSpinCount = 0;
    if (options.extraSpin !== 'n')
        extraSpinCount = 100;
    else extraSpinCount = 1;

    while (playResponse.ended && playResponse.buyExtraSpin.extraSpin && extraSpinCount-- && options.extraSpin !== 'n') {
        console.log(options.raw
            ? beautify(playResponse)
            : beautify(options.truncate ? getTruncatedResp(playResponse): playResponse, null, 2, 100)
        );

        const { buyExtraSpin } = playResponse;

        console.log("\n");

        let { activeFeatures, freeSpins } = playResponse;
        let activeFeatStr = ``;
        activeFeatures.map ((af) => activeFeatStr += `${af}, `);

        let wildsColSoFar = freeSpins[freeSpins.length-1].wsCollectedSoFar || 0;
        wildsColSoFar = (wildsColSoFar > 12 ? 12 : wildsColSoFar);

        console.table({
            ...buyExtraSpin,
            prevWinAmount: playResponse.winAmount,
            activeFeatStr,
            wildsColSoFar,
        });

        let answer;
        if (options.extraSpin) {
            answer = options.extraSpin;
        } else {
            answer = await rl.question("Buy extra spin? (y/n) > ");
        }

        const extraSpinStr = answer.trim().toLowerCase() === 'y';
        console.log( colors.green(`\nExtra Spin => ${extraSpinStr ? "Buy" : "Exit"}\n`));

        if (!extraSpinStr)
            return null;

        if (playResponse.persistent)
            playParams.options.prevState = playResponse;

        playParams.options.extraSpin = extraSpinStr;
        playParams.buyFeature        = true;
        playParams.betAmount         = buyExtraSpin.expectedBet;
        playParams.featureMode       = buyExtraSpin.featureMode;
        playResponse                 = GameWrapper.play (playParams);

        if (!options.extraSpin)
            extraSpinCount = 1;
    }

    console.table({
        ...playResponse.buyExtraSpin,
        prevWinAmount: playResponse.winAmount,
    });

    checkNegativeValues (playResponse);
    return playResponse;
}

function getTruncatedResp(pr) {
    if (!pr)
        return {};

    const cloned = { ...pr };
    if (pr.freeSpins && Array.isArray(pr.freeSpins)) {
        cloned.freeSpins = pr.freeSpins.map(fs => ({
            ...fs,
            initialSymbols:      [{ truncated: true }],
            intermediateSymbols: [{ truncated: true }],
            stoppedGrid:         [{ truncated: true }],
            symbols:             [{ truncated: true }],
            winnings:            [{ truncated: true }],
        }));
    }

    cloned.internal = {};

    return cloned;
}

main ()
    .then ((playResponse) => {
        if (playResponse)
            console.log (options.raw ?
                beautify (playResponse)
                : beautify (playResponse, null, 2, 100)
            );

        process.stdout.end ();
        process.exit (0);
    })
    .catch ((err) => {
        console.error (colors.red ('Error:'), err);
        process.exit (1);
    })
;
