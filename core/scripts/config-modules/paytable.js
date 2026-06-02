const PAYTABLE_CONFIG = require("./data/paytable");


function matchPaytable (refPaytable, givenPaytable) {
    if (refPaytable.length !== givenPaytable.length) {
        return `paytable length mismatch - expected ${refPaytable.length} entries, got ${givenPaytable.length}`;
    }

    for (let i = 0; i < refPaytable.length; i++) {
        const refEntry = refPaytable[i];
        const givenEntry = givenPaytable[i];

        if (!givenEntry) {
            return `paytable entry[${i}] missing`;
        }

        if (refEntry.symbol !== givenEntry.symbol) {
            return `paytable entry[${i}] symbol incorrect - should be ${refEntry.symbol}, got ${givenEntry.symbol}`;
        }

        if (refEntry.pay !== givenEntry.pay) {
            return `paytable entry[${i}] pay incorrect - should be ${refEntry.pay}, got ${givenEntry.pay} for symbol ${refEntry.symbol}`;
        }

        if (refEntry.count !== givenEntry.count) {
            return `paytable entry[${i}] count incorrect - should be ${refEntry.count}, got ${givenEntry.count} for symbol ${refEntry.symbol}`;
        }
    }
}


function func (config) {
    const { paytable } = config;

    if (!paytable) {
        return `field "paytable" not found`;
    }

    const result = matchPaytable(PAYTABLE_CONFIG, paytable);
}

module.exports = {
    name: 'Paytable Test',
    func: func,
};