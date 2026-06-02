function func (config, playParams, response) {
    if (playParams.betAmount !== response.betAmount)
        return 'betAmount does not match';

    if (playParams.currencyMultiplier !== response.currencyMultiplier)
        return 'currencyMultiplier does not match';

    if (response.winAmount !== (response.refWinAmount * response.currencyMultiplier))
        return `response.winAmount !== response.refWinAmount * response.currencyMultiplier)`;

    if (playParams.featureMode !== response.featureMode)
        return 'featureMode does not match';

    if (response.winAmount < 0)
        return `negative play-response winAmount(${ response.winAmount }), possible overflow`;

    if ((response.fsStatus.length > 0 && !response.subgameTriggered) || (response.fsStatus.length === 0 && response.subgameTriggered))
        return `fsStatus.length = ${response.fsStatus.length} but subgameTriggered = ${response.subgameTriggered}`;

    if (response.maxWinTriggered && response.winAmount !== (config.maxWinCap * response.refBetBase * response.currencyMultiplier))
        return `max-win-triggered but response.winAmount = ${response.winAmount} != (config.maxWinCap * response.refBetBase * response.currencyMultiplier)=${config.maxWinCap * response.refBetBase * response.currencyMultiplier}`;

    return null;
}

module.exports = {
    name: 'Basic Tests',
    func: func,
};
