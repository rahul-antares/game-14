/* global process */

const keys2Print = [
    "gameId",
    "roundId",
    "betType",
    "betAmount",
    "spinTime",
    "spinType",
    "numBS",
    "gambleSuccess",
    "wildBlockSize",
    "weightSetFreeSpins",
    "weightSetRandomFeature",
    "reelsetIndex",
    "stops",
    "randomFeature",
    "randomFeatureSize",
    "randomFeaturePosition",
    "randomFeatureTease",
    "teaseSize",
    "yFavor",
    "wrReplacement",
    "extraSpinChoice",
    "extraSpinConfig",
    "winSaver",
    "winAmount",
];

function printHeader () {
    for (let key of keys2Print)
    process.stdout.write (key + '\t');

    process.stdout.write ('\n');
}

function printRow (round, header) {
    if (header)
        printHeader ();

    for (let spin of round) {
        for (let key of keys2Print) {
            const value = spin[key];
            if (value === null || value === undefined)
                continue;

            switch (key) {
                case 'stops':
                    process.stdout.write (value.reduce ((acc, curr, index) => acc + curr + (index === value.length - 1 ? "": ", "), "") + '\t');
                    break;

                case 'spinTime':
                    if (!value)
                        process.stdout.write (value + '\t');
                        else
                        process.stdout.write (value.toISOString() + '\t');
                    break;

                case 'gambleSuccess':
                    process.stdout.write (value.reduce ((acc, curr, index) => acc + (curr) + (index === value.length - 1 ? "": ","), "") + '\t');
                    break;

                default:
                    process.stdout.write (value + '\t');
                    break;
            }
        }

        process.stdout.write ('\n');
    }
}

module.exports = { printRow };
