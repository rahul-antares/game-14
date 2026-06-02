const commonKeys = [ 'winAmount', 'refWinAmount' ];

const playRespKeys = [
    ...commonKeys, 'betAmount',
    'currencyMultiplier', 'refBetAmount',
    'refBetBase', 'featureMode',
];

const spinKeys = [
    ...commonKeys, 'winsSoFar',
    'refWinsSoFar', 'baseWinAmount',
    'refBaseWinAmount', 'numSC',
];

function negativeCheck (key, value) {
    if (value < 0)
        return `${key} has negative value = ${value}`;

    return null;
}

function checkNegativeValues (response) {
    // play-resp
    validate (response, playRespKeys);

    // base-spin
    validate (response.baseSpin, spinKeys);

    // free spins
    for (let fs of response.freeSpins) {
        validate (fs, spinKeys);
    }
}

function validate (resp, keys) {
    for (let key of keys) {
        if ((key === 'stops' || key === 'symbols') && resp[key].length === 0) {
            handleErr (resp, 'empty stops/symbols');
        } else if (typeof resp[key] === 'number') {
            const negcheck = negativeCheck (resp[key]);
            if (negcheck)
                handleErr (resp, negcheck);
        } else if (key === 'reelSet' && !resp[key]) {
            handleErr (resp, 'empty reelset');
        }
    }
}

function handleErr (obj, msg) {
    console.error (obj);
    throw new Error (msg);
}

module.exports = {
    checkNegativeValues
};
