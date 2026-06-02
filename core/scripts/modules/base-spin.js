function func (config, playParams, response) {
    const { maxWinCap }                               = config;
    const { betAmount, currencyMultiplier, baseSpin } = response;

    if (baseSpin.winAmount !== (baseSpin.refWinAmount * currencyMultiplier))
        return `base-spin win amount doesn't equal to base-spin refwin-amount * currency-multiplier`;

    // if (response.fsStatus.length === 0 && baseSpin.winAmount !== response.winAmount)
    //     return `no free spins yet base-spin win amount is not equal to response win amount`;

    // if (response.fsStatus.length === 0 && baseSpin.winsSoFar !== response.winAmount)
    //     return `no free spins yet base-spin win-so-far is not equal to response win amount`;

    if (baseSpin.stops.length == 0 && baseSpin.winAmount > 0)
        return `no stops but win in base-spin`;

    if (baseSpin.winnings.length === 0 && baseSpin.winAmount > 0)
        return `empty winnings array, but win in base-spin`;

    let err = null;

    return err;
}

module.exports = {
    name: 'Base Spin Tests',
    func: func,
};
