const PAYLINES = require("./data/paylines")

function func (config) {
    const { paylines } = config;

    if(paylines.length !== 15) {
        return `Number of Paylines is incorrect: expected 11 , got ${paylines.length}`;
    }

    for(let j = 0 ; j < paylines.length; j++) {
        const payline = paylines[j];
        if(payline.length !== 5) {
            return `Payline length cannot be ${payline.length}`;
        }
        for (let i = 0; i < 5; i++) {
            if (payline[i] !== PAYLINES[j][i]) {
                return `# ${j} payline, at index ${i} payline value mismatch , ${PAYLINES[j][i]}, shoudl be ${payline[i]}`;
            }
        }
    }

    return null;
}

module.exports = {
    name:'Paylines Tests',
    func: func,
}