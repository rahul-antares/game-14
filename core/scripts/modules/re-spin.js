function func (config, playParams, response) {
    let err = null;

    const { baseSpin, fsStatus } = response;
    const { reSpins } = baseSpin;
    const bottomRowIdx = 2;

    if (reSpins.length == 0) {
        const { symbols } = baseSpin;
        let bottomRow = false;

        for (let i = 0; i < symbols.length; i++) {
            if (symbols[i][bottomRowIdx] === 'SC') {
                bottomRow = true;
                break;
            }
        }

        if (baseSpin.numSC === 2 && baseSpin.scatterHook === false && bottomRow === false)
            err = `no re-spins awarded, with 2 scatters and no scatter-hook feature`;
        else
            return err;
    }

    if (reSpins.length > 0 && baseSpin.numSC !== 2)
        return `re-spins awarded when scatter count != 2, #scatter-count = ${baseSpin.numSC}`;

    if (fsStatus.length > 0 && reSpins.length > 0 && reSpins[reSpins.length-1].numSC < 3)
        return `free-spins awarded using re-spins(#size = ${reSpins.length}), last-re-spin-scatters = ${reSpins[reSpins.length-1].numSC}`;

    err = reSpinReelSetCheck (baseSpin);
    if (err !== null)
        return err;

    return err;
}

function reSpinReelSetCheck (baseSpin) {
    const { reSpins } = baseSpin;

    if (reSpins.length === 0)
        return null;

    const rsReelSet = reSpins[0].reelSet;
    if (reSpins.length > 1 &&  rsReelSet !== reSpins[1].reelSet)
        return `re-spins have two different reelsets, reel-set1 = ${rsReelSet}, reel-set2 = ${reSpins[1].reelSet}`;

    return null;
}

const rsReelSetPerc = {
    rsIdxOne: 0,
    sameAsBaseGame: 0,
    set1: 0,
    restSets: 0,
    total: 0,
};

function getReelSetProb(baseSpin, mode) {
    if (!baseSpin || !baseSpin.reSpins || baseSpin.reSpins.length === 0) return;

    rsReelSetPerc.total += 1;

    const rsReelSet = baseSpin.reSpins[0].reelSet;
    if (rsReelSet === baseSpin.reelSet) {
        rsReelSetPerc.sameAsBaseGame = (rsReelSetPerc.sameAsBaseGame || 0) + 1;
    } else {
        rsReelSetPerc.rsIdxOne = (rsReelSetPerc.rsIdxOne || 0) + 1;
    }

    const isSetOne = (
        (mode == 0 && rsReelSet === 'RG_reelSet1') ||
        (mode == 1 && rsReelSet === 'BB_reelSet1') ||
        (mode == 2 && rsReelSet === 'SB_reelSet1')
    );

    if (isSetOne) {
        rsReelSetPerc.set1 += 1;
    } else {
        rsReelSetPerc.restSets += 1;
    }
}

function printRsReelsetPerctable (obj) {
    const entries = Object.entries(obj);
    const countFieldsSum = (keys) =>
        keys.reduce((s, k) => {
            const v = Number(obj[k] ?? 0);
            return s + (Number.isFinite(v) ? v : 0);
        }, 0);

    const explicitTotal = Number(obj.total);
    const hasExplicitTotal = Number.isFinite(explicitTotal);

    const reelSetKeys = entries
        .map(([k]) => k)
        .filter((k) => /^[A-Za-z]{1,3}_/.test(k)); // e.g. RG_, XG_, etc.
    const reelSetSum = countFieldsSum(reelSetKeys);

    const allNumericSum = entries.reduce((s, [, v]) => s + (Number(v) || 0), 0);

    const total = hasExplicitTotal ? explicitTotal : (reelSetSum > 0 ? reelSetSum : allNumericSum);

    const tableData = entries.map(([key, value]) => {
        const numeric = Number(value) || 0;
        const pct = total === 0 ? '0.00%' : ((numeric / total) * 100).toFixed(2) + '%';
        return { Key: key, Values: numeric, '%': pct };
    });

    console.table(tableData);
}

module.exports = {
    name: 'Respin Tests',
    func: func,
    printRsReelsetPerctable,
    getReelSetProb,
};
