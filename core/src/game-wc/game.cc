#include <cstdint>
#include <iostream>
#include <stdexcept> // std::invalid_argument
#include <vector>
#include "game.h"
#include "constants.h"
#include "data-defs.h"
#include "symbols.h"
#include "../utils/log.h"
#include "../utils/error.h"

GameWithCheats::GameWithCheats() : Game() {
    reelSet = new ReelSetWC(rng);
    createBetMap();
}

GameWithCheats::~GameWithCheats() {}

#define MAX_ITERATION 1000000
PlayResponse GameWithCheats::play(PlayOptions options, Cheats cheats) {
    PlayResponse response;
    int counter = 0;


    do {
        response = actuallyPlay (options, cheats);
        if (counter >= MAX_ITERATION)
            throw std::overflow_error("iteration limit reached"); 

    } while (!this->gotDesiredOutcome (response, cheats, counter++));

    return response;
}

int GameWithCheats::getNumScatters () {
    return rng.getWeightedIndex(NUM_SCATTERS, 3) + 3;
}

PlayResponse GameWithCheats::actuallyPlay(PlayOptions options, Cheats& cheats) {
    PlayResponse  playResponse;
    PayWindowWC   payWindow(rng, *((ReelSetWC *)reelSet));

    int refBetBase = getRefBetBase (options);

    playResponse.betAmount          = options.betAmount;
    playResponse.refBetBase         = refBetBase;
    playResponse.featureMode        = options.featureMode;
    playResponse.currencyMultiplier = options.currencyMultiplier;
    playResponse.refBetAmount       = refBetBase * options.currencyMultiplier;
    playResponse.action             = "spin";

    RoundWC round(rng, playResponse, payWindow, cheats);


    try {
        if (options.options.extraSpin) {
            round.runExtraSpins (options.options.prevState);
        } else {
            round.play ();
        }

        collectRands (playResponse);
        playResponse.refWinAmount = calculateWins (playResponse);
        applyCurrencyMultiplier (playResponse);
        extraSpinsCondition (playResponse);
    }
    catch (const std::exception& e) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Exception caught%s:\n     %s\n", ANSI_COLOR_RESET, e.what());
        throw;
    }
    catch (...) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Unknown Exception caught: %s\n", ANSI_COLOR_RESET);
        throw;
    }

    return playResponse;
}

PlayResponse GameWithCheats::next(NextPlay next, PlayResponse prev, Cheats cheats) {
    PlayResponse response;
    int counter = 0;

    do {
        response = actuallyNext (next, prev, cheats);

        if (counter >= MAX_ITERATION)
            throw std::overflow_error("iteration limit reached"); 

    } while (!this->gotDesiredOutcomeNext (next, response, cheats, counter++));

    return response;
}

bool hasMatchingCoinValue (const std::vector<CoinSymbol>& csyms, const int64_t coinValue) {
    for (auto& cs: csyms)
        if (cs.valueX == coinValue)
            return true;

    return false;
}


bool GameWithCheats::gotDesiredOutcomeNext (NextPlay& next, PlayResponse& response, Cheats& cheats, int counter) {
    FsStatus lastFsStatus    = response.fsStatus.back();
    int level                = lastFsStatus.level;
    int status               = lastFsStatus.status;
    int maxFSLevel           = 3;

    bool matchWinCap       = (cheats.winCap < 0           || this->evaluateWinCap (response, cheats, counter));
    bool matchFsMaxwin     = (cheats.maxFSWin < 0         || this->evaluateMaxFsWin (response, cheats, counter, true));
    bool matchFsMinwin     = (cheats.minFSWin < 0         || this->evaluateMinFsWin (response, cheats, counter));
    bool matchNumFeatures  = (cheats.numFeatures < 0      || this->evaluateNumFeatures (response, cheats, counter));
    bool matchFeatures     = (cheats.features.size() == 0 || this->evaluateFeatures (response, cheats, counter));

    bool matchFsCoinValue  = (cheats.fsCoinValue < 0      || this->evaluateFsCoinValue (response, cheats, counter));
    bool matchFsDoubleWild = (cheats.fsDoubleWild < 0     || this->evaluateFsDoubleWild (response, cheats, counter));
    bool matchFsCoinValueCollected  = (cheats.fsCoinValueCollected < 0 || this->evaluateFsCoinValueCollected (response, cheats, counter));
    bool matchFsCoinValueEveryLevel  = (cheats.fsCoinValueEveryLevel < 0      || this->evaluateFsCoinValueEveryLevel (response, cheats, counter));
    bool matchFsCoinValueCollectedEveryLevel  = (cheats.fsCoinValueCollectedEveryLevel < 0 || this->evaluateFsCoinValueCollectedEveryLevel (response, cheats, counter));

    bool matchMaxCollectionLevel  = (cheats.maxCollectionLevel < 0) || this->evaluateMaxCollectionLevel (response, cheats, counter, true);
    bool matchMinCollectionLevel  = (cheats.minCollectionLevel < 0) || this->evaluateMinCollectionLevel (response, cheats, counter, true);
    bool matchGambleSuccessLevel  = (cheats.gambleSuccessLevel < 0 || this->evaluateGambleSuccessLevel (next, response, cheats, counter));

    return (
        matchFsMaxwin &&
        matchFsMinwin &&
        matchWinCap &&
        matchNumFeatures &&
        matchFeatures &&
        matchMaxCollectionLevel &&
        matchMinCollectionLevel &&
        matchFsDoubleWild &&
        matchFsCoinValue &&
        matchFsCoinValueCollected &&
        matchFsCoinValueEveryLevel &&
        matchFsCoinValueCollectedEveryLevel &&
        matchGambleSuccessLevel
    );
}

PlayResponse GameWithCheats::actuallyNext(NextPlay next, PlayResponse prev, Cheats cheats) {
    PlayResponse playResponse;
    PayWindowWC    payWindow(rng, *((ReelSetWC *)reelSet));

	playResponse.betAmount          = prev.betAmount;
    playResponse.refBetBase         = prev.refBetBase;
    playResponse.refBetAmount       = prev.refBetBase * prev.currencyMultiplier;
	playResponse.currencyMultiplier = prev.currencyMultiplier;
	playResponse.featureMode        = prev.featureMode;
	playResponse.action             = next.gamble ? "gamble":"collect";
	playResponse.baseSpin           = prev.baseSpin;
	playResponse.subgameTriggered   = prev.subgameTriggered;
	playResponse.winAmount          = prev.winAmount;
	playResponse.refWinAmount       = prev.refWinAmount;
	playResponse.fsStatus           = prev.fsStatus;

    RoundWC round(rng, playResponse, payWindow, cheats);

    try {
        round.next(next.gamble);
        collectRands(playResponse);
        playResponse.refWinAmount = calculateWins (playResponse);
        applyCurrencyMultiplier(playResponse);
    }
    catch (const std::exception& e) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Exception caught%s:\n     %s\n", ANSI_COLOR_RESET, e.what());
        throw;
    }
    catch (...) {
        fprintf(stderr, ANSI_BG_COLOR_RED "Unknown Exception caught: %s\n", ANSI_COLOR_RESET);
        throw;
    }

    return playResponse;
}

bool GameWithCheats::gotDesiredOutcome(PlayResponse& response, Cheats& cheats, int counter) {
    if (response.featureMode >= MODE_EXTRA_SPIN_START)
        return true; // NOTE: until the es-cheats are not implemented

    bool matchMinWin      = (cheats.minWin < 0            || this->evaluateMinWin (response, cheats, counter));
    bool matchMaxWin      = (cheats.maxWin < 0            || this->evaluateMaxWin (response, cheats, counter));
    bool matchWinSymbol   = (cheats.winSymbol.empty()     || this->evaluateWinSym (response, cheats, counter));
    bool matchExcSymbol   = (cheats.excludeSymbol.empty() || this->evaluateExcludeSym (response, cheats, counter));
    bool matchPayline     = (cheats.paylineWin < 0        || this->evaluatePaylineWin (response, cheats, counter));
    bool matchReelset     = (cheats.reelset < 0           || this->evaluateReelset (response, cheats, counter));
    bool matchCoinValue   = (cheats.coinValue < 0         || this->evaluateCoinValue (response, cheats, counter));
    bool matchNumCoins    = (cheats.numCoins < 0          || this->evaluateNumCoins (response, cheats, counter));
    bool matchFreeSpins   = (cheats.freespins < 0         || this->evaluateFreeSpins (response, cheats, counter));
    bool matchNumScatters = (cheats.numScatters < 0       || this->evaluateNumScatters (response, cheats, counter));
    bool matchReSpins     = (cheats.respin < 0            || this->evaluateReSpin (response, cheats, counter));
    bool matchFsMaxwin    = (cheats.maxFSWin < 0          || this->evaluateMaxFsWin (response, cheats, counter, false));
    bool matchScatterHook = (cheats.scatterHook < 0       || this->evaluateScatterHook (response, cheats, counter));
    bool matchTopRowRespin = (cheats.topRowRespin < 0     || this->evaluateTopRowReSpin (response, cheats, counter));
    bool matchMaxCollectionLevel  = (cheats.maxCollectionLevel < 0)       || this->evaluateMaxCollectionLevel (response, cheats, counter, false);
    bool matchMinCollectionLevel  = (cheats.minCollectionLevel < 0)       || this->evaluateMinCollectionLevel (response, cheats, counter, false);

    LOG (
        "[cheats:gotDesiredOutcome] final: %d %d %d %d %d %d %d %d %d %d\n",
        matchMinWin,
        matchMaxWin,
        matchWinSymbol,
        matchExcSymbol,
        matchPayline,
        matchReelset,
        matchCoinValue,
        matchNumCoins,
        matchFreeSpins,
        matchReSpins
    );

    return (
        matchMinWin &&
        matchMaxWin &&
        matchWinSymbol &&
        matchExcSymbol &&
        matchPayline &&
        matchReelset &&
        matchFreeSpins &&
        matchFsMaxwin &&
        matchNumCoins &&
        matchCoinValue &&
        matchNumScatters &&
        matchReSpins &&
        matchScatterHook &&
        matchTopRowRespin &&
        matchMaxCollectionLevel &&
        matchMinCollectionLevel
    );
}

bool GameWithCheats::evaluateTopRowReSpin (PlayResponse& response, Cheats& cheats, int counter) {
    return response.baseSpin.reSpins.size() == 2 && cheats.topRowRespin > 0;
}

bool GameWithCheats::evaluateFeatures (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    if (cheats.features.size() > response.activeFeatures.size())
        return false;

    bool matched = true;
    for (auto i = 0; i < cheats.features.size(); i++) // order maintained
        matched = matched & (FS_FEATURE_FROM_STR (cheats.features[i]) == response.activeFeatures[i]);

    return matched;
}


bool GameWithCheats::evaluateNumFeatures (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    return response.activeFeatures.size() == cheats.numFeatures;
}

bool GameWithCheats::evaluateNumCoins (PlayResponse& response, Cheats& cheats, int counter) {
    return response.baseSpin.coinSyms.size() == cheats.numCoins;
}

bool GameWithCheats::evaluateCoinValue (PlayResponse& response, Cheats& cheats, int counter) {
    return hasMatchingCoinValue(response.baseSpin.coinSyms, cheats.coinValue);
}

bool GameWithCheats::evaluateScatterHook (PlayResponse& response, Cheats& cheats, int counter) {
    return response.baseSpin.scatterHook && cheats.scatterHook > 0;
}

bool GameWithCheats::evaluateRespinsRetrigger (PlayResponse& response, Cheats& cheats, int counter) {
    bool matched = false;

    for (auto rs: response.baseSpin.reSpins) {
        if ((rs.wildSyms.size() - rs.lockedSymbols.size()) > 0) // means we got a new ws
            return true;
    }

    return matched;
}

bool GameWithCheats::evaluateFsDoubleWild (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    for (auto& fs: response.freeSpins)
        if (fs.newWsLanded >= 2)
            return true;

    return false;
}

bool GameWithCheats::evaluateFsCoinValueEveryLevel (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    auto prevLevel = 0;

    for (auto& fs: response.freeSpins) {
        if (fs.level <= prevLevel)
            continue;

        bool coinValueMatch = hasMatchingCoinValue (fs.coinSyms, cheats.fsCoinValueEveryLevel);
        if (coinValueMatch && fs.level > prevLevel)
            prevLevel = fs.level;
        else
            return false;
    }

    return true;
}

bool GameWithCheats::evaluateFsCoinValueCollectedEveryLevel (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    auto prevLevel = 0;

    for (auto& fs: response.freeSpins) {
        if (fs.level <= prevLevel)
            continue;

        bool coinValueMatch = hasMatchingCoinValue (fs.coinSyms, cheats.fsCoinValueCollectedEveryLevel);
        if (fs.newWsLanded > 0 && coinValueMatch && fs.level > prevLevel)
            prevLevel = fs.level;
        else
            return false;
    }

    return true;
}

bool GameWithCheats::evaluateFsCoinValue (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    for (auto& fs: response.freeSpins) {
        for (auto& cs: fs.coinSyms) {
            if (cs.valueX == cheats.fsCoinValue)
                return true;
        }
    }

    return false;
}

bool GameWithCheats::evaluateFsCoinValueCollected (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    for (auto& fs: response.freeSpins) {
        for (auto& cs: fs.coinSyms) {
            if (cs.valueX == cheats.fsCoinValueCollected && fs.newWsLanded > 0)
                return true;
        }
    }

    return false;
}

bool GameWithCheats::evaluateMinCollectionLevel (PlayResponse& response, Cheats& cheats, int counter, const bool inNext) {
    if (response.freeSpins.size() == 0)
        return true;

    bool colLevelMatch = (
        response.freeSpins.size() > 0 &&
        response.freeSpins.back().level >= cheats.minCollectionLevel
    );

    if ((cheats.minFSWin > 0 || cheats.numScatters > 0) && !inNext)
        return true;

    return colLevelMatch;
}

bool GameWithCheats::evaluateMaxCollectionLevel (PlayResponse& response, Cheats& cheats, int counter, const bool inNext) {
    if (response.freeSpins.size() == 0)
        return true;

    bool colLevelMatch = (
        response.freeSpins.size() > 0 &&
        response.freeSpins.back().level <= cheats.maxCollectionLevel
    );

    if ((cheats.minFSWin > 0 || cheats.numScatters > 0) && !inNext)
        return true;

    return colLevelMatch;
}

bool GameWithCheats::evaluateNumScatters (PlayResponse& response, Cheats& cheats, int counter){
    return (response.baseSpin.numSC == cheats.numScatters);
}

/* Ola: maxFSWin is overall maximum win including base spin, evaluated in freeSpins */
bool GameWithCheats::evaluateMaxFsWin (PlayResponse& response, Cheats& cheats, int counter, bool inNext) {
    double win = (double) response.winAmount / (double) response.betAmount;

    uint64_t baseWinAmount = response.baseSpin.winAmount;
    for(auto rs: response.baseSpin.reSpins) {
        baseWinAmount += rs.winAmount;
    }

    double baseWin = (double) baseWinAmount / (double) response.betAmount;
    bool max       = false;

    // check for this condition as it can cause the game to loop untill the iterations
    // get exhausted
    if (inNext && baseWin >= cheats.maxFSWin)
        max = true;
    else if (win < cheats.maxFSWin)
        max = true;

    if (max) {
        LOG("[%d] maxFsWin evaluation = true [multiplier = %0.3f]\n", counter, win);
    }

    return max;
}

/* Ola: minFSWin is overall minimum win including base spin, evaluated in freeSpins */
bool GameWithCheats::evaluateMinFsWin (PlayResponse& response, Cheats& cheats, int counter) {
    double winX = (double) response.winAmount / (double) response.betAmount;
    bool min = false;

    if (winX >= cheats.minFSWin)
        min = true;

    if (min) {
        LOG("[%d] minFsWin evaluation = true [multiplier = %0.3f]\n", counter, win);
    }

    return min;
}

bool GameWithCheats::evaluateWinCap (PlayResponse& response, Cheats& cheats, int counter) {
    if (response.freeSpins.size() == 0)
        return true;

    bool expectedMatch = cheats.winCap == 1;
    bool matched = false;
    bool maxWinTriggered = response.maxWinTriggered;

    if (!expectedMatch && !maxWinTriggered)
        matched = true;

    if (expectedMatch && maxWinTriggered)
        matched = true;

    if (matched) {
        LOG("[%d] matched evaluation = true\n", counter);
    }

    return matched;
}

bool GameWithCheats::evaluateGambleSuccessLevel(NextPlay& next, PlayResponse& response, Cheats& cheats, int counter) {
    FsStatus lastFsStatus = response.fsStatus.back();
    int level = lastFsStatus.level, status = lastFsStatus.status;
    const int thresholdLevel = cheats.gambleSuccessLevel;

    if (!next.gamble)
        return true;

    if (level <= thresholdLevel)
        return status == INIT;

    if (level == thresholdLevel + 1)
        return status == INIT || status == FAILED || status == COLLECTED;

    if (level == thresholdLevel + 2)
        return status == FAILED;

    return false;
}

bool GameWithCheats::evaluateFreeSpins(PlayResponse& response, Cheats& cheats, int counter) {
    bool expectedMatch = (cheats.freespins == 1);
    bool matched = false;

    if (!expectedMatch && response.fsStatus.size() == 0)
        matched = true;

    if (expectedMatch && response.fsStatus.size() != 0)
        matched = true;

    if (matched) {
        LOG("[%d] freespins matched\n", counter);
    }

    return matched;
}

int getReelSetNum (const std::string& reelset) {
    if (reelset.size() < 2)
        throw_error ("reelset must have at characters, reelset = ", reelset);

    auto secondLast = reelset.size()-2;
    if (reelset[secondLast]-'0' > 10)
        return reelset.back()-'0';

    return std::stoi (reelset.substr(secondLast, 2));
}

bool GameWithCheats::evaluateReelset (PlayResponse& response, Cheats& cheats, int counter) {
    if (cheats.reelset <= 0 || cheats.reelset > 11)
        throw_error ("invalid range of reel-sets, should be between [1 - 11]");

    return getReelSetNum(response.baseSpin.reelSet) == cheats.reelset;
}

bool GameWithCheats::evaluateReSpin (PlayResponse& response, Cheats& cheats, int counter) {
    return (
        ( response.baseSpin.reSpins.size() > 0 && cheats.respin > 0) ||
        ( response.baseSpin.reSpins.size() == 0 && cheats.respin == 0)
    );
}

// bool GameWithCheats::evaluateNumCoins (PlayResponse& response, Cheats& cheats, int counter) {
//     int coinCount = 0;

//     for (auto symArr : response.baseSpin.symbols) {
//         for (auto sym: symArr) {
//             if (sym == CS && sym != INVALID)
//                 coinCount++;
//         }
//     }

//     return coinCount == cheats.numCoins;
// }

bool GameWithCheats::evaluateMinWin(PlayResponse& response, Cheats& cheats, int counter) {
    double win = (double) response.winAmount / (double) response.betAmount;
    bool min = false;

    if (win >= cheats.minWin)
        min = true;

    if (min) {
        LOG("[%d] minWin evaluation = true [multiplier = %0.3f]\n", counter, win);
    }

    return min;
}

/*Cheat: Max Win*/
bool GameWithCheats::evaluateMaxWin(PlayResponse& response, Cheats& cheats, int counter) {
    double win = (double) response.winAmount / (double) response.betAmount;
    bool max = false;

    if (win <= cheats.maxWin)
        max = true;

    if (max) {
        LOG("[%d] maxWin evaluation = true [multiplier = %0.3f]\n", counter, win);
    }

    return max;
}

bool GameWithCheats::evaluatePaylineWin(PlayResponse& response, Cheats& cheats, int counter) {
    bool match = false;

    for (auto i : response.baseSpin.winnings) {
        if (i.payLine == cheats.paylineWin) {
            match = true;
            break;
        }
    }

    if (match) {
        LOG("[%d] paylineWin evaluation = %d\n", counter, match);
    }

    return match;
}

bool GameWithCheats::evaluateWinSym(PlayResponse& response, Cheats& cheats, int counter) {
    bool match = false, countMatch = !!(cheats.winSymbolCount < 0);
    Symbol winSymbol = getSymbol (cheats.winSymbol);

    for (auto win : response.baseSpin.winnings) {
        if (win.symbol == winSymbol) {
            match = true;

            if (cheats.winSymbolCount >= 0) {
                if (win.symCount == cheats.winSymbolCount)
                    countMatch = true;
            }

            if (match && countMatch)
                break;

            match = false;
        }
    }

    if (match && countMatch) {
        LOG("[%d] winSymbol evaluation = %d (count = %d)\n", counter, match, countMatch);
    }

    return match && countMatch;
}

bool GameWithCheats::evaluateExcludeSym(PlayResponse& response, Cheats& cheats, int counter) {
    bool match = true;
    Symbol excludeSymbol = getSymbol (cheats.excludeSymbol);

    // excludes the symbol from base-spin and re-spin(base-spin) winnings
    for (auto win : response.baseSpin.winnings) {
        if (win.symbol == excludeSymbol) {
            match = false;
            break;
        }
    }

    if (match) {
        LOG("[%d] excludeSymbol evaluation = %d\n", counter, match);
    }

    return match;
}

bool GameWithCheats::evaluateStops(PlayResponse& response, Cheats& cheats, int counter) {
    int count = 0;
    bool match = false;

    if (count == cheats.stops.size())
        match = true;


    if (match) {
        LOG("[%d] stops evaluation = %d\n", counter, match);
    }

    return true; // true, since we are using forced stops now
}
