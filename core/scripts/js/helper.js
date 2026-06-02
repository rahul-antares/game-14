const { createInterface }                         = require('node:readline/promises');
const colors                                      = require('colors');

const rl = createInterface({
    input: process.stdin,
    output: process.stdout,
});

function isValidIndex (index, picked, start, end) {
    const pickedIndex = parseInt(index);
    if (pickedIndex < start || pickedIndex > end || isNaN(pickedIndex))
        return {
            pickedIndex,
            valid: false,
            msg: `Invalid index chosen, range must be between ${start} and ${end}` 
        };

    //TODO: these checks on the game engine side
    for (const p of picked) {
        if (pickedIndex === p)
            return {
                pickedIndex,
                valid: false,
                msg: `Index ${pickedIndex} already chosen, choose a different index between ${start} and ${end}` 
            };
    }

    return { pickedIndex, valid: true, msg: '' }
}

async function prompt() {
    let input = '';

    do {
        input = await rl.question (colors.yellow('\ngamble(g) or collect(c) > '));
        console.log ('input = ', input)
    }
    while (input !== 'c' && input !== 'g');

    console.log ();

    return input;
}

async function promptBoosterPick(alreadyPicked = [], left, total, endIdx = 15) {
    const takenIndices = (alreadyPicked || []).map(p => p.pickIndex);
    const takenMsg = takenIndices.length
        ? `Already picked indexes: [${takenIndices.join(', ')}]\n`
        : '';

    const awardPickMsg = `Picks (Awarded = ${total}, Picked = ${total - left}, Left = ${left})\n`;
    const promptMsg = `${awardPickMsg}Pick a booster index between 1 and 15\n${takenMsg}`;

    let input, shouldAskAgain = true;
    while (shouldAskAgain) {
        input = await rl.question(`\n${colors.yellow(promptMsg)}> `);
        const {pickedIndex, valid, msg } = isValidIndex(input, takenIndices, 1, endIdx);
        if (valid)
            return pickedIndex;

        shouldAskAgain = !valid;
        console.log(colors.red(`${msg}\nInput Index = ${pickedIndex}`));
    }

    return input;
}

module.exports = { isValidIndex, promptBoosterPick, prompt, rl };
