const REELSETS = require('./data/reels.js');


function matchReels (spinType, set, refReels, givenReels) {
    for (let i = 0; i < refReels.length; i++) {
        const refReel   = refReels[i];
        const givenReel = givenReels[i];

        for (let j = 0; j < refReel.length; j++) {
            if (refReel[j] !== givenReel[j])
                return `${spinType}.${set} reel[${i}] symbol(${givenReel[j]}) incorrect - should be ${refReel[j]} (position # ${j})`
        }
    }

    return null;
}

function func (config) {
    const { reelsets } = config;

    if (!reelsets)
        return `field "reelsets" not found`;

    for (let spinType in REELSETS) {
        if (!reelsets[spinType])
            return `reelset for "${spinType}" missing`;

        const sets = REELSETS[spinType];
        const given = reelsets[spinType];

        for (let set in sets) {
            if (!given[set])
                return `set ${set} missing`;

            const refReels   = sets[set];
            const givenReels = given[set];
            const result     = matchReels (spinType, set, refReels, givenReels);
            if (result)
                return result;
        }
    }

    return null;
}

module.exports = {
    name:'Reel Sets Test',
    func: func,
};