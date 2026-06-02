const { BG_ReelSets, FS_ReelSets, RF_ReelSets } = require('../config-modules/data/reel-sets');

const bgReelSetKeys = Object.keys(BG_ReelSets);
const rfReelSetKeys = Object.keys(RF_ReelSets);
const fsReelSetKeys = Object.keys(FS_ReelSets);

function func (config, playParams, response) {
    return null;
}

function getReel(stop, reelSet, reelLen = 5) {
    if (stop < 0)
        return `invalid stop value = ${stop} for reelSet = ${reelSet}`;

    const stops = [];
    for (let i = 0; i < reelLen; i++) {
        const symIdx = (i + stop + reelSet.length) % reelSet.length;
        stops.push (reelSet[symIdx]);
    }

    return stops;
}

module.exports = {
    name: 'Reel Set Tests',
    func: func,
};
