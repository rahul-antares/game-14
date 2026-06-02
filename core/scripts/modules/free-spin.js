function func (config, playParams, response) {
    const { activeFeatures } = response;
    let prevWilds = 0;

    for (const af of activeFeatures) {
        if (af === "START_ON_LEVEL_2") {
            prevWilds = 4;
            break;
        }
    }

    let removeLowestActive = false;
    for (let af of response.activeFeatures) {
        if (af === "REMOVE_LOWEST") {
            removeLowestActive = true;
            break;
        }
    }


    let err = null;
    for (const fs of response.freeSpins) {
        if (prevWilds + fs.newWsLanded !== fs.wsCollectedSoFar)
            return `FS = ${fs.index}, prevWilds(${prevWilds}) + newWsLanded(${fs.newWsLanded}) !== wsCollectedSoFar(${fs.wsCollectedSoFar})`

        prevWilds = fs.wsCollectedSoFar;

        err = testRemoveLowest(fs, removeLowestActive);
        if (err)
            return err;
    }

    return err;
}

function testRemoveLowest (fs, removeLowest=false) {
    if (!removeLowest)
        return null;

    let idxMsg = `index = ${fs.index}: `;
    let err = null;
    for (const x of fs.coinSymbols) {
        if (x.costX === 2) {
            err = `${idxMsg} remove_lowest active but contains 2x coin symbol`;
            break;
        }
    }

    return err;
}

function wildHookCheck (fs) {
    let wsCount = 0;
    let idxMsg = `index = ${fs.index}: `;

    for (const symg of fs.symbols) {
        for (const sym of symg) {
            if (sym === "WS")
                wsCount++;
        }
    }

    if (fs.wildSymbols.length !== fs.newWsLanded || fs.wildSymbols.length !== wsCount)
        return `${idxMsg} inconsistent occurence of wild symbols in free-spin`;

    if (wsCount > 1 && fs.wildHookGrid.length > 0)
        return `${idxMsg} wild hook feature active with two wilds`;

    if (wsCount !== fs.wildSymbols.length)
        return `${idxMsg} total no. of wilds not equal to #elements in wild symbols array`;

    return null;
}

module.exports = {
    name: 'Free Spin Tests',
    func: func,
};
