#!/bin/env node

/* global process */

const colors                = require('colors');
const { program }           = require('commander');
const beautify              = require("json-beautify");
const { generateRoundData } = require('./generate');
const { printRow }          = require('./print');

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
    .option('-e, --extraSpin <extra-spin>', 'extra-spin buy', false)
    .option('-y, --buyFeature', 'buy feature', false)
    .option('-m, --currencyMultiplier <value>', 'currency multiplier value, default value is 1', myParseInt, 1)
    .option('-f, --featureMode <mode>', 'feature mode', parseFloat, 0)
    .option('-g, --gamble <gamble>', 'true(gamble) or (false)collect', 'c')
    .option('-G, --gamblePercentage <value>', 'gamble probability', parseFloat, 100)
    .option('-v, --variant <variant>', '96 or 94', '96')
    .option('-i, --iterations <iterations>', 'iterations', 1000000)
    .option('--specialGamble <level-six/seven-gamble-percentage>', 'gamble-percentage', parseFloat, 100)
    .parse (process.argv)
;

const options = program.opts ();
const { GameWrapper } = require(`../../index${options.variant}`);

if (!options.betAmount) {
    program.outputHelp();
    process.exit(0);
}

if (isNaN(options.gamblePercentage) || options.gamblePercentage < 0 || options.gamblePercentage > 100) {
    console.error (`Invalid value(${options.gamblePercentage}) for gamble percentage, should be in range [0, 100]`);
    process.exit();
}

const playParams = {
    betAmount          : options.betAmount,
    buyFeature         : options.buyFeature,
    featureMode        : options.featureMode,
    currencyMultiplier : options.currencyMultiplier || 1, // this causes modification in betAmount
    options            : {
        extraSpin: false,
    }
};

function getGambleProb(fsStatus) {
    if (!fsStatus || fsStatus.length <= 0) {
        console.error ("invalid fs-status", { fsStatus });
        throw new Error ("invalid fs-status");
    }


    const level = fsStatus[fsStatus.length - 1].level;
    if (level === 6 || level === 7) {
        return options.specialGamble / 100;
    }

    return options.gamblePercentage / 100;
}

function play (playParams) {
    let playResponse;
    let stage = 'play';

    try {
        playResponse = GameWrapper.play (playParams);
        stage = 'next';

        while (!playResponse.ended) {
            const gambleProb = getGambleProb(playResponse.fsStatus);
            const gamble     = Math.random() < gambleProb;
            playResponse     = GameWrapper.next ({ gamble }, playResponse);
        }
    }
    catch (err) {
        console.error (colors.red ('GameWrapper error:'), `failed at state "${stage}"`);
        console.error (beautify(playResponse, null, 2, 100));
        throw err;
    }

    return playResponse;
}

function pruneConfig (config) {
    let modeName;

    for (let key in config.betConfigs) {
        if (config.betConfigs[key].mode === playParams.featureMode)
            modeName = key
    }

    return { ...config, modeName };
}

async function main () {
    let response;

    GameWrapper.initRNG ({});
    const _config = GameWrapper.getConfig();
    const config  = pruneConfig (_config);

    for (let i = 1; i <= options.iterations; i++) {
        response = play (playParams);
        const data = generateRoundData ({ config, playParams, response, roundId: i });

        if (i%10000 === 0) {
            process.stderr.write (`${i.toLocaleString()} records written ...\n`);
        }

        printRow (data, i === 1);
    }
}

main ()
    .then (() => {
        process.exit (0);
    })
    .catch ((err) => {
        console.error (colors.red ('Error:'), err);
        process.exit (1);
    })
;
