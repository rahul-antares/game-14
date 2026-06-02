const normalBetSizes   = [ 10, 20, 30, 50, 80, 100, 150, 200, 300, 500, 800, 1000, 1250, 1500, 2000, 3000, 5000, 8000, 10000, 12500, 15000, 20000, 30000, 50000, 80000, 100000, ];
const enhancedBetSizes = [ 15, 30, 45, 75, 120, 150, 225, 300, 450, 750, 1200, 1500, 1875, 2250, 3000, 4500, 7500, 12000, 15000, 18750, 22500, 30000, 45000, 75000, 120000, 150000, ];
const FB1BetSizes      = [ 550, 1100, 1650, 2750, 4400, 5500, 8250, 11000, 16500, 27500, 44000, 55000, 68750, 82500, 110000,165000, 275000, 440000, 550000, 687500, 825000, 1100000, 1650000, 2750000, 4400000, 5500000];
const FB2BetSizes      = [ 1050, 2100, 3150, 5250, 8400, 10500, 15750, 21000, 31500, 52500, 84000, 105000, 131250, 157500, 210000, 315000, 525000, 840000, 1050000, 1312500, 1575000, 2100000, 3150000, 5250000, 8400000, 10500000 ];
const FB3BetSizes      = [ 1950, 3900, 5850, 9750, 15600, 19500, 29250, 39000, 58500, 97500, 156000, 195000, 243750, 292500, 390000, 585000, 975000, 1560000, 1950000, 2437500, 2925000, 3900000, 5850000, 9750000, 15600000, 19500000 ];

const regular = {
    bets: normalBetSizes,
    defaultMax : 20000,
    rtp: 96,
    mode: 0,
    type: 'regular',
    maxWin: 5000,
    cost: 10,
}

const bonusboost = {
    bets: enhancedBetSizes,
    defaultMax: 30000,
    rtp: 96,
    mode: 1,
    type: "boost",
    maxWin: 3333.3,
    cost: 15,
}

const fb1 = {
    bets: FB1BetSizes,
    defaultMax: 1100000,
    rtp: 96,
    mode: 2,
    type: 'feature',
    maxWin: 90.9,
    cost: 550,
}

const fb2 = {
    bets: FB2BetSizes,
    defaultMax: 2100000,
    rtp: 96,
    mode: 3,
    type: 'feature',
    maxWin: 47.6,
    cost: 1050,
}

const fb3 = {
    bets: FB3BetSizes,
    defaultMax: 3900000,
    rtp: 96,
    mode: 4,
    type: 'feature',
    maxWin: 25.6,
    cost: 1950,
}

const BConfig = {
    regular,
    bonusboost,
    fb1, fb2, fb3
}

function checkBetSizes (betName, ref, artefact) {
    if (ref.length !== artefact.length)
        return `betConfigs[${betName}].bets.length (${artefact.length}) mismatch - should be ${ref.length}`;

    for (let i = 0; i < ref.length; i++)
        if (ref[i] !== artefact[i])
            return `betConfigs[${betName}].bet entry ${ref[i]} does not match (is ${artefact[i]})`;

    return null;
}

function func (config) {
    const { betConfigs } = config;

    if(! betConfigs )
        return `field "betConfigs" not found`;

    for(let key in BConfig) {
        const ref = BConfig[key];
        const artefact = betConfigs[key];

        if(!artefact)
            return `betConfigs[${key}] is missing`;
        for(let subKey in ref) {
            switch (subKey) {
                case 'bets': {
                    const s = checkBetSizes(key, ref.bets, artefact.bets);
                    if(s) return s;
                    break;
                }

                default:
                    if(ref[subKey] !== artefact[subKey])
                        return `betConfigs[${key}].${subKey}(${artefact[subKey]} mismatch - should be ${ref[subKey]})`;
            }
        }
    }

    return null;
}

module.exports = {
    name: 'Bet Config Tests',
    func: func,
}