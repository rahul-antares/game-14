#include <cstdint>
#include <stdexcept>
#include <map>
#include "bets.h"
#include "extra-spin.h"
#include "sstream"
#include <vector>
#include "../utils/log.h"
#include "game.h"
#include "round.h"
#include "paywindow.h"
#include "reelset.h"
#include "../utils/colors.h"


Game::Game() {
    reelSet = new ReelSet(rng);
    createBetMap();
}

Game::~Game(){}; 

void Game::initRNG(RngOpts& rngOpts) {
    rng.init(rngOpts);
}

void Game::reseed() {
    return rng.reseed();
}

GameConfig Game::getConfig() {
    GameConfig config;

    config.creditsPerBet = CREDITS_PER_BET;
    config.maxWinCap     = MAX_WIN_CAP;
    config.rgsets        = reelSet -> getRGSet();
    config.bbsets        = reelSet -> getBBSet();
    config.sbsets        = reelSet -> getSBSet();
    config.fssets        = reelSet->getFSSet();

    return config;
}

void Game::extraSpinsCondition (PlayResponse& resp) {
    const auto& esmeta = resp.buyExtraSpin;
    const auto respEs  = resp.extraSpin;
    const auto& feat   = resp.featureMode;

    if (feat >= MODE_EXTRA_SPIN_START) {
        if (!respEs)
            throw_error ("extra spin feature but the resp.extraSpin = ", respEs);

        if (esmeta.featureMode > 0 && esmeta.costX > 0 && !esmeta.extraSpin)
            throw_error ("extra spin buy info present, but not the option to purchase it");
        if (esmeta.featureMode < 0 && esmeta.costX < 0 && esmeta.extraSpin)
            throw_error ("extra spin buy info absent, but option to purchase it is present");
    } else if (feat < MODE_EXTRA_SPIN_START) {
        if (respEs)
            throw_error ("not extra spin feature but the resp.extraSpin = ", respEs);
        if (esmeta.featureMode > 0 && esmeta.costX > 0 && resp.freeSpins.empty())
            throw_error ("extra spin buy info present without free-spins in non-es-mode");
    }
}

PlayResponse Game::play(PlayOptions options) {
    PlayResponse playResponse;
    PayWindow    payWindow(rng, *reelSet);

    LOG("---------------------------------------------------\n");
    LOG("play arguments:\n");
    LOG("%s    betAmount          =%s %ld\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, options.betAmount);
    LOG("%s    featureMode        =%s %d\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, options.featureMode);
    LOG("%s    buyFeature         =%s %d\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, options.buyFeature);
    LOG("%s    currencyMultiplier =%s %ld\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, options.currencyMultiplier);
    LOG("---------------------------------------------------\n");

    _play(playResponse, options, payWindow);
    applyCurrencyMultiplier (playResponse);
    extraSpinsCondition (playResponse);

    return playResponse;
}

void Game::_play(PlayResponse& response, PlayOptions& opts, PayWindow& payWindow) {
    try {

        response.betAmount          = opts.betAmount;
        response.featureMode        = opts.featureMode;
        response.currencyMultiplier = opts.currencyMultiplier;
        response.refBetBase         = getRefBetBase (opts);
        response.refBetAmount       = response.refBetBase * opts.currencyMultiplier;

        Round round(rng, response, payWindow);
        if (opts.options.extraSpin) {
            round.runExtraSpins (opts.options.prevState);
            response.extraSpin = true;
        } else {
            round.play ();
        }

        collectRands(response);
        response.refWinAmount = calculateWins(response);
    }
    catch (const std::exception& e) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Exception caught%s:\n     %s\n", ANSI_COLOR_RESET, e.what());
        throw;
    }
    catch (...) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Unknown Exception caught: %s\n", ANSI_COLOR_RESET);
        throw;
    }
}

PlayResponse Game::next(NextPlay next, PlayResponse prev) {
    PlayResponse playResponse;
    PayWindow    payWindow(rng, *reelSet);

    LOG("---------------------------------------------------\n");
    LOG("next arguments:\n");
    LOG("%s    next.gamble             =%s %d\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, next.gamble);
    LOG("%s    prev.betAmount          =%s %ld\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, prev.betAmount);
    LOG("%s    prev.featureMode        =%s %d\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, prev.featureMode);
    LOG("%s    prev.currencyMultiplier =%s %ld\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, prev.currencyMultiplier);
    LOG("---------------------------------------------------\n");

    playResponse.betAmount          = prev.betAmount;
    playResponse.refBetBase         = prev.refBetBase;
    playResponse.currencyMultiplier = prev.currencyMultiplier;
    playResponse.featureMode        = prev.featureMode;
    playResponse.refBetAmount       = prev.refBetBase * prev.currencyMultiplier;
    playResponse.action             = next.gamble ? "gamble":"collect";
    playResponse.baseSpin           = prev.baseSpin;
    playResponse.subgameTriggered   = prev.subgameTriggered;
    playResponse.winAmount          = prev.winAmount;
    playResponse.refWinAmount       = prev.refWinAmount;
    playResponse.fsStatus           = prev.fsStatus;

    Round round(rng, playResponse, payWindow);

    try {
        round.next(next.gamble);
        collectRands(playResponse);

        playResponse.refWinAmount = calculateWins(playResponse);
    }
    catch (const std::exception& e) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Exception caught%s:\n     %s\n", ANSI_COLOR_RESET, e.what());
        throw;
    }
    catch (...) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Unknown Exception caught: %s\n", ANSI_COLOR_RESET);
        throw;
    }

    applyCurrencyMultiplier(playResponse);
    return playResponse;
}

int64_t Game::calculateWins( const PlayResponse& playResponse) {
    int64_t refWinAmount = 0;
    int64_t maxWinAmount = MAX_WIN_CAP * playResponse.refBetBase;

    refWinAmount += playResponse.baseSpin.refWinAmount;

    // base spin respins
    for(auto rs: playResponse.baseSpin.reSpins)
        refWinAmount += rs.refWinAmount;

    // free spin
    for (auto freeSpin : playResponse.freeSpins) {
        refWinAmount += freeSpin.refWinAmount;

        // free spin respins
        // for(auto rs: freeSpin.reSpins)
        //     refWinAmount += rs.refWinAmount;
    }

    if (refWinAmount > maxWinAmount)
        throw_error ("winAmount(", refWinAmount, ") > maxWinAmount(", maxWinAmount, ")");

    return (refWinAmount >= maxWinAmount) ? maxWinAmount : refWinAmount;
}

int64_t Game::getRefBetBase (PlayOptions &options) {
    int64_t betAmount          = options.betAmount;
    int64_t currencyMultiplier = options.currencyMultiplier;
    int featureMode            = options.featureMode;
    int buyFeature             = options.buyFeature;
    bool extraSpin             = options.options.extraSpin;

    if (betmap.count((BetMode) featureMode) == 0)
        throw_error ("invalid featureMode = ", featureMode); 

    if ((!buyFeature && featureMode != 0) ||
        (buyFeature && featureMode == 0))
        throw std::invalid_argument("incompatible feature mode and buy");

    auto betSizeMap         = betmap.at ((BetMode)featureMode);
    auto normalizedBetAmount = betAmount / currencyMultiplier;

    if (betSizeMap.count (normalizedBetAmount) == 0)
        throw std::invalid_argument("invalid bet size");

    if (extraSpin) {
        if (featureMode < MODE_EXTRA_SPIN_START || featureMode >= MODE_EXTRA_SPIN_END)
            throw std::logic_error ("invalid extra spin mode");

        // validateExtraSpinBuy (options);

        const EsBetConfig& esConfig = ES_BET_CONFIGS [featureMode - MODE_EXTRA_SPIN_START];
        if (esConfig.cost == 0)
            throw_error("should not happen, invalid extra spin cost: zero value");

        return normalizedBetAmount * CREDITS_PER_BET / esConfig.cost;

    } else if ((BetMode) featureMode > MODE_FEATURE_BUY5) {
        throw std::logic_error ("incompatible feature mode and extra spin");
    }

    BetConfig config = ALL_BETS_CONFIG [featureMode];
    return normalizedBetAmount * CREDITS_PER_BET / config.cost;
}

void Game::collectRands(PlayResponse& r) {
    rng.getAndClearHistory(r.internal.rHistory);
}

void Game::applyCurrMultiSpin (Spin& s, int64_t currMp) {
    s.winAmount      = s.refWinAmount * currMp;
    s.winsSoFar      = s.refWinsSoFar * currMp;
    s.coinSymbolWins = s.refCoinSymbolWins * currMp;
    s.baseWinAmount  = s.refBaseWinAmount * currMp;

    for (auto &cs: s.coinSyms)
        cs.value = cs.refValue * currMp;

    for(auto &w: s.winnings)
        w.winAmount = w.refWinAmount * currMp;
}

void Game::applyCurrencyMultiplier(PlayResponse& r) {
    const int64_t currMp = r.currencyMultiplier;

    // play response
    r.winAmount    = r.refWinAmount * currMp;

    // base-spin
    applyCurrMultiSpin (r.baseSpin, currMp);

    for(auto &rs: r.baseSpin.reSpins) {
        applyCurrMultiSpin (rs, currMp);
    }

    // free-spins
    for(auto &fs: r.freeSpins) {
        applyCurrMultiSpin (fs, currMp);

        for(auto &rs: fs.reSpins) {
            applyCurrMultiSpin (rs, currMp);
        }
    }

    // buy extra spin
    if (r.buyExtraSpin.expectedBet >= 0)
        r.buyExtraSpin.expectedBet *= currMp;
}

void Game::createBetMap() {
    std::map<int64_t, bool> tempBetMap;

    for (int64_t i : bet_NORMAL)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_NORMAL, tempBetMap });

    tempBetMap.clear();
    for (int64_t i : bet_ENHANCED)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_ENHANCED, tempBetMap });

    tempBetMap.clear();
    for (int64_t i : bet_SB)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_SB, tempBetMap });

    tempBetMap.clear();
    for (int64_t i : bet_FB1)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_FEATURE_BUY1, tempBetMap });

    tempBetMap.clear();
    for (int64_t i : bet_FB2)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_FEATURE_BUY2, tempBetMap });

    tempBetMap.clear();
    for (int64_t i : bet_FB3)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_FEATURE_BUY3, tempBetMap });

    tempBetMap.clear();
    for (int64_t i : bet_FB4)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_FEATURE_BUY4, tempBetMap });

    tempBetMap.clear();
    for (int64_t i : bet_FB5)
        tempBetMap.insert ({ i, true });
    betmap.insert ({ MODE_FEATURE_BUY5, tempBetMap });

    // Extra Spins
    for (const EsBetConfig& betConf : ES_BET_CONFIGS) {
        tempBetMap.clear();

        for (int64_t i : betConf.allowed)
            tempBetMap.insert ({ i, true });

        betmap.insert({ (BetMode)betConf.mode, tempBetMap });
    }
}
