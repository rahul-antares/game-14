const { max } = require("moment");

function _gen ({ config, betAmount, betType, spin, response, roundId, fsStatus }) {
    const { rfeat, rfTease, rfSize, rfPos } = getRandomFeatureValues (spin, spin.type === "base game");
    const isFreeSpin = (spin.type === "free spin");

    const r = {
        gameId                  : config.gameCore + config.variant,
        roundId,
        betType                 : betType || '',
        betAmount               : betAmount || '',
        spinTime                : new Date(),
        winAmount               : spin.winAmount,
        spinType                : spin.type,
        numBS                   : spin.numBS,
        gambleSuccess           : getGambleSuccess (fsStatus),
        wildBlockSize           : getWildBlockSize (spin, isFreeSpin),
        weightSetFreeSpins      : (isFreeSpin ? spin.weightSet : ""),
        weightSetRandomFeature  : (spin.type === "base game" ? spin.weightSet : ""),
        reelsetIndex            : spin.reelSet || '',
        stops                   : spin.stops,
        randomFeature           : rfeat,
        randomFeatureSize       : rfSize,
        randomFeaturePosition   : rfPos,
        randomFeatureTease      : rfTease,
        teaseSize               : getTeaseSize (spin, spin.type === "base game"),
        yFavor                  : (isFreeSpin ? spin.yFavor : ""),
        wrReplacement           : getWrReplacement (spin, isFreeSpin),
        winSaver                : getWinSaver(spin, isFreeSpin),
        extraSpinChoice         : '',
        extraSpinConfig         : '',
    };

    if (r.randomFeature === "TRUE" && r.randomFeatureTease !== "TRUE")
        throw new Error ("random feature tease not true when random feature true");

    return r;
}

const gambleStatus = {
    INIT      : "INIT",
    FAILED    : "FAILED",
    SUCCEEDED : "SUCCEEDED",
    COLLECTED : "COLLECTED",
};

function getWildBlockSize (spin, isFs) {
    if (!isFs)
        return "";

    return `{${spin.wsWalk.size.x},${spin.wsWalk.size.y}}`;
}

function getWinSaver (spin, fs = false) {
    if (!fs)
        return ``;
    return spin.winSaver ? "TRUE": "FALSE";
}

function getWrReplacement (spin, fs = false) {
    if (!fs)
        return '';

    let str = ``;
    const { wrSymbols, symbols } = spin;

    for (let i = 0; i < wrSymbols.length; i++) {
        const wrs = wrSymbols[i];
        const replacementSym = symbols[wrs.x][wrs.y];
        str += `{${wrs.x},${wrs.y};${replacementSym}}`
        if (i < wrSymbols.length-1)
            str += ',';
    }

    return str;
}

function getTeaseSize (spin, basegame = false) {
    if (!basegame || spin.wsWalkTeaseBlocks.length < 3)
        return '';

    const { wsWalkTeaseBlocks } = spin;

    let str = '';

    for (let i = 0; i < wsWalkTeaseBlocks.length; i++) {
        const { size } = wsWalkTeaseBlocks[i];
        str += `{${size.x},${size.y}}`;
        if (i < wsWalkTeaseBlocks.length-1)
            str += ',';
    }

    return str;
}

function getRandomFeatureValues (spin, basegame=false) {
    const rfValues = {
        rfeat: "",
        rfTease: "",
        rfSize: "",
        rfPos: "",
    };

    if (!basegame)
        return rfValues;

    rfValues.rfeat = spin.randomFeature ? "TRUE": "FALSE";
    rfValues.rfTease = spin.wsWalkTeaseBlocks.length === 3 ? "TRUE": "FALSE";

    if (spin.randomFeature) {
        const { size, origin } = spin.wsWalk;
        rfValues.rfSize = `{${ size.x }, ${ size.y }}`;
        rfValues.rfPos  = `{${ origin.x }, ${origin.y}}`;
    }

    return rfValues;
}

function getGambleSuccess (fsStatus) {
    if (!fsStatus || fsStatus.length === 0)
        return [];

    const gambleS = [null, null, null, null, null, null];
    const maxGambleLevel = 6;

    for (const fss of fsStatus) {
        if (fss.level >= maxGambleLevel)
            break;

        if (fss.status === gambleStatus.SUCCEEDED) {
            gambleS[fss.level-1] = true;
        } else if (fss.status == gambleStatus.FAILED) {
            gambleS[fss.level-1] = false;
        }
    }

    if (gambleS.length > maxGambleLevel)
        throw new Error ("gambleS length > ", maxGambleLevel);

    gambleS.pop();

    return gambleS;
}

function getBetType (bt) {
    let bType = "";

    switch (bt) {
        case "fb1": bType += "free spins level 1"; break;
        case "fb2": bType += "free spins level 2"; break;
        case "fb3": bType += "free spins level 3"; break;
    }

    return (bType === "" ? bt : bType);
}

function generateRoundData (opts) {
    const data             = [];

    data.push (
        _gen ({
            ...opts,
            betAmount : opts.playParams.betAmount,
            betType   : getBetType (opts.config.modeName) || opts.config.modeName,
            spin      : { ...opts.response.baseSpin},
            fsStatus  : opts.response.fsStatus,
        })
    );

    for (let fs of opts.response.freeSpins) {
        data.push (
            _gen ({
                ...opts,
                spin: fs,
            })
        );
    }

    return data;
}

module.exports = { generateRoundData }
