function parseBool (input) {
    switch (input) {
        case 'true':
        case '1':
            return true;

        case 'false':
        case '0':
            return false;

        default:
            throw new Error (`could not parse boolean "${input}"`);
    }
}

function parseString (input) {
    return input;
}

function parseStringArray(input) {
    if (!input)
        return null;

    return input.split(',');
}

function parseIntArrayOfArrays(input) {
    const arr2d = [];
    const arrays = input.split(']');

    for (const arrStr of arrays) {
        const res = arrStr.split('[').filter((arr) => arr.length > 1);
        if (res.length < 1)
            continue;

        arr2d.push(parseIntArray(res[0]));
    }

    return arr2d;
}

function parseDoubleArrayOfArrays(input) {
    const arr2d = [];
    const arrays = input.split(']');

    for (const arrStr of arrays) {
        const res = arrStr.split('[').filter((arr) => arr.length > 1);
        if (res.length < 1)
            continue;

        arr2d.push(parseDoubleArray(res[0]));
    }

    return arr2d;
}

function parseDouble(value) {
    const parsedValue = parseFloat(value, 10);
    if (isNaN(parsedValue)) {
        throw new program.InvalidArgumentError('Not a number.');
    }
    return parsedValue;
}

function parseDoubleArray(input) {
    if (!input)
        return [];

    const arr = input.split(',');
    const resArr = [];

    for (const curr of arr) {
        try {
            const parsedCurr = parseDouble(curr);
            resArr.push(parsedCurr);
        } catch (e) {
            continue;
        }
    }

    return resArr;
}

function parseIntArray (input) {
    if (!input)
        return [];

    const arr = input.split(',');
    const resArr = [];
    for (const curr of arr) {
        const parsedCurr = parseInt(curr);
        if (isNaN(parsedCurr))
            continue;

        resArr.push(parsedCurr);
    }

    return resArr;
}

function parseBoolArray (input) {
    if (!input)
        return [];

    const arr = input.split(',');
    return arr.map ((curr) => {
        return parseBool (curr);
    });
}

const Parser = {
    'double'               : parseDouble,
    'int'                  : parseInt,
    'string'               : parseString,
    'bool'                 : parseBool,
    'boolean'              : parseBool,
    'array<int>'           : parseIntArray,
    'array<string>'        : parseStringArray,
    'array<bool>'          : parseBoolArray,
    'array<double>'        : parseDoubleArray,
    'array<array<int>>'    : parseIntArrayOfArrays,
    'array<array<double>>' : parseDoubleArrayOfArrays,
};

module.exports = {
    parseStringArray,
    parseDouble,
    parseIntArrayOfArrays,
    parseBool,
    parseString,
    parseIntArray,
    parseBoolArray,
    Parser,
};

