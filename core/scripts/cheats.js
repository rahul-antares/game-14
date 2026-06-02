const { 
    parseBool,
    parseString,
    parseIntArray,
    parseBoolArray,
    parseIntArrayOfArrays,
    parseStringArray,
    parseDouble,
} = require('./js/parsers');

const CheatsList = [
    // IMPLEMENTED?  NAME               PARSE METHOD          DESCRIPTION
    [ true,  'minWin',             parseDouble,          'the minimum base game win, expressed as a bet multiplier' ],
    [ true,  'maxWin',             parseDouble,          'the maximum base game win, expressed as a bet multiplier' ],
    [ true,  'winSymbol',          parseString,          'the symbol type that generates a win' ],
    [ true,  'numBonus',           parseInt,             'the symbol type that generates a win' ],
    [ true,  'winSymbolCount',     parseInt,             'the number of above given symbol' ],
    [ true,  'excludeSymbol',      parseString,          'symbol to be excluded from win' ],
    [ true,  'paylineWin',         parseInt,             'any win on the given pay line (0 - 14) in the base spin' ],
    [ true,  'freespins',          parseBool,            'guarantees landing at least 3 SC during a spin' ],
    [ true,  'numScatters',        parseInt,             'exact number of SC landing during a spin' ],
    [ true,  'stops',              parseIntArray,        'the stop positions for the five reels' ],
    [ true,  'randomFeature',      parseBool,            'triggers the random scatter feature' ],
    [ true,  'randomFeatureSize',  parseIntArray,        'triggers the random scatter tease' ],
    [ true,  'minFSWin',           parseDouble,          'the minimum win in free spins, expressed as a bet multiplier' ],
    [ true,  'maxFSWin',           parseDouble,          'the maximum win in free spins, expressed as a bet multiplier' ],
    [ true,  'finalSize',          parseIntArray,        'finalSize' ],
    [ true,  'reachedSizes',       parseIntArrayOfArrays,'reachedSizes' ],
    [ true,  'gambleSuccessLevel', parseInt,             'gamble success level' ],
    [ true,  'winCap',             parseBool,            'win cap' ],
];

const CheatsMap = {};

for (let row of CheatsList) {
    const implemented = row[0];
    const name        = row[1];
    const parser      = row[2];
    const desc        = row[3];
    // might wanna add the approach(forced, random-draw, etc)

    CheatsMap[name] = {
        implemented, name, parser, desc
    };
}

// module.exports = { CheatsList, CheatsMap };


