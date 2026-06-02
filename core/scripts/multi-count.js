#!/bin/env node

const os                  = require('os');
const fs                  = require('fs');
const path                = require('path');
const colors              = require('colors');
const sprintf             = require('sprintf-js').sprintf;
const { program }         = require('commander');
const beautify            = require("json-beautify");
const { CheatsMap }       = require("./cheats");
const { all } = require('axios');

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
    .option('-y, --buyFeature', 'buy feature', false)
    .option('-m, --currencyMultiplier <value>', 'currency multiplier value, default value is 1', myParseInt, 1)
    .option('-f, --featureMode <mode>', 'feature mode', parseFloat, 0)
    .option('-g, --gamble <gamble>', 'gamble(g) or (collect)c', 'c')
    .option('-i, --include <module...>', 'relative path of UT modules to include (can specify multiple options)')
    .option('-x, --exclude <module...>', 'relative path of UT modules to exclude (can specify multiple options)')
    .option('-v, --variant <variant>', '96 or 94', '96')
    .option('-V, --verbose', 'verbose')
    .option('-l, --list-modules', 'list modules')
    .option('-q, --quiet', 'show only errors')
    .option('--raw', 'dump raw JSON for post processing', false)
;

Object.keys(CheatsMap).forEach((name) => {
    const { implemented, parser, desc } = CheatsMap[name];

    program.option (`--${name} <value>`, (!implemented ? colors.red('(NOT IMPLEMENTED) ') : '') + desc, parser);
})

program.parse (process.argv);

const utModulesDir = path.join(__dirname, 'modules');
const cheatsModuleDir = path.join(__dirname,'modules/cheats');
const options = program.opts ();

const cheats = {};
const cheatNames = Object.keys (CheatsMap);

for (let argName in options) {
    if (cheatNames.indexOf(argName) >= 0)
        cheats[argName] = options[argName];
}

const usingCheats = Object.keys (cheats).length > 0;
const indexFile   = `../index${ options.variant }${usingCheats? '-wc': ''}`;
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
};

function hasCheats(cheats) {
    return (Object.keys(cheats).find((curr) => typeof cheats[curr] !== 'undefined') !== null);
}

if (hasCheats(cheats))
    playParams.cheats = cheats;

const tests = [];

function matchPath (pathArr, file) {
    return pathArr.find ((p) => {
        const regExp = new RegExp (`${file}$`)
        if (p.match (regExp))
            return true;
        return false;
    });
}

function play (playParams) {
    let playResponse;
    let stage = 'play';

    try {
        playResponse = GameWrapper.play (playParams);

        while (!playResponse.ended || playResponse.triggeredAction !== "none") {

            const triggeredAction = playResponse.triggeredAction;

            const nextPlayOpts = {};

            if (triggeredAction == "gamble") {
                const gamble = options.gamble.trim() == 'g';
                nextPlayOpts['action'] = gamble ? 'gamble' : 'collect';
                nextPlayOpts['gamble'] = gamble;
                playResponse = GameWrapper.next (nextPlayOpts, playResponse);
            }
        }

        // let esCount = 0;
        // for (let b of playResponse.pickClickGame.boosters.picked) {
        //     if (b.startsWith("EXTRASPIN")) {
        //         esCount += 1;
        //     }
        // }
        //
        // if (esCount === playResponse.pickClickGame.totalPicks-1) {
        //     console.error ({ ...playResponse.pickClickGame });
        // }
    }
    catch (err) {
        console.error (colors.red ('GameWrapper error:'), `failed at state "${stage}"`);
        console.error (beautify(playResponse, null, 2, 100));
        throw err;
    }

    return playResponse;
}

let count = 0;
async function main () {
    let response;

    console.error ('using RNG API', colors.yellow(GameWrapper.getRNGLibType().type));

    // loadTests ();
    // loadCheatTests ();

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

    const config = GameWrapper.getConfig();

    const allBoosters = [
        "MULTI1X"        ,
        "MULTI2X"        ,
        "MULTI3X"        ,
        "WILDREEL2"      ,
        "WILDREEL3"      ,
        "WILDREEL4"      ,
        "EXTRASPINS2"    ,
        "EXTRASPINS3"    ,
        "EXTRASPINS4"    ,
        "EXTRASPINS5"    ,
        "EXTRAWILDH1"    ,
        "EXTRAWILDH2"    ,
        "EXTRAWILDH3"    ,
        "EXTRAWILDH4"    ,
        "EXTRAWILDH5"    ,
        "LOWTOHIGHL6H1"  ,
        "LOWTOHIGHL7H2"  ,
        "LOWTOHIGHL8H3"  ,
        "LOWTOHIGHL9H4"  ,
        "LOWTOHIGHL10H5" ,
        "EXTRAPICK"      ,
    ];

    const boosters = {
        initialTotal: 0,
        retriggerTotal: 0,
        totalCombined: 0,
        "MULTI1X"        : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "MULTI2X"        : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "MULTI3X"        : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "WILDREEL2"      : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "WILDREEL3"      : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "WILDREEL4"      : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRASPINS2"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRASPINS3"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRASPINS4"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRASPINS5"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRAWILDH1"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRAWILDH2"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRAWILDH3"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRAWILDH4"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRAWILDH5"    : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "LOWTOHIGHL6H1"  : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "LOWTOHIGHL7H2"  : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "LOWTOHIGHL8H3"  : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "LOWTOHIGHL9H4"  : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "LOWTOHIGHL10H5" : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
        "EXTRAPICK"      : { initial: 0, retrigger: 0, initialP: 0, retriggerP: 0, totalP: 0, total: 0 },
    };

    while (true) {
        response = play (playParams);

        count++;
        const { freeSpins } = response;

        for (const pb of response.pickClickGame.boosters.picked) {
            boosters[pb].initial += 1;
            boosters.initialTotal += 1;
            boosters[pb].total += 1;
        }

        for (const fs of freeSpins) {
            for (const pb of fs.pickClickGame.boosters.picked) {
                boosters[pb].retrigger += 1;
                boosters.retriggerTotal += 1;
                boosters[pb].total += 1;
            }
        }

        boosters.totalCombined = boosters.retriggerTotal + boosters.initialTotal;

        for (let b of allBoosters) {
            boosters[b].initialP   = (boosters[b].initial / boosters.initialTotal).toFixed(3);
            boosters[b].retriggerP = (boosters[b].retrigger / boosters.retriggerTotal).toFixed(3);
            boosters[b].total      = boosters[b].initial + boosters[b].retrigger;
            boosters[b].totalP     = (boosters[b].total / boosters.totalCombined).toFixed(3);
        }

        if (!(count % 50000)) {
            const memTotal = parseFloat((os.totalmem() / 1024 / 1024).toFixed(2)).toLocaleString() + ' MB';
            const memFree  = parseFloat((os.freemem()  / 1024 / 1024).toFixed(2)).toLocaleString() + ' MB';
            console.log (`+ ${count.toLocaleString()} iterations (mem = ${colors.yellow(memFree)} / ${memTotal})`);

            writeBoostersToCSV(boosters, "booster-count.csv");
        }
    }
}

function writeBoostersToCSV(boosters, fileName) {
    const tmpPath = path.join(os.tmpdir(), fileName);
    console.log({ tmpPath });

    const headers = ['Booster', 'initial', 'retrigger', 'initialP', 'retriggerP', 'totalP', 'total'];
    const lines = [];

    // Add headers
    lines.push(headers.join(','));

    // Add overall totals
    lines.push(`TOTALS,${boosters.initialTotal},${boosters.retriggerTotal},,, ,${boosters.totalCombined}`);

    // Add each booster
    for (const [key, value] of Object.entries(boosters)) {
        if (typeof value !== 'object') continue;

        const row = [
            key,
            value.initial ?? 0,
            value.retrigger ?? 0,
            value.initialP ?? 0,
            value.retriggerP ?? 0,
            value.totalP ?? 0,
            value.total ?? 0,
        ];
        lines.push(row.join(','));
    }

    // Write all at once
    fs.writeFileSync(tmpPath, lines.join('\n'), 'utf8');
    console.log(`CSV successfully written to ${tmpPath}`);
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
