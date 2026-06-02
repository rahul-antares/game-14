#include <cstdint>
#include <utility>
#include "./round.h"
#include "../utils/log.h"
#include "../utils/error.h"
#include "bets.h"
#include "constants.h"
#include "data-defs.h"
#include "extra-spin.h"
#include "symbols.h"

Round::Round (
        RngLib& rng,
        PlayResponse& playResponse,
        PayWindow& payWindow
        )
    :
        rng(rng),
        playResponse(playResponse),
        payWindow(payWindow),
        mode(playResponse.featureMode)
{
    payWindow.setRoundRef(this);
    clear ();
}

inline int numSCToFsLevel (const int numSC) { return numSC < 3 ? -1 : numSC - 2; }

void Round::play () {
    clear ();

    int numSC = 0;

    playResponse.ended = true;
    playResponse.empty = false;

    if (mode >= MODE_FEATURE_BUY1) {
        playResponse.fsStatus.push_back({
                .level  = mode - (int)MODE_FEATURE_BUY1 + 1,
                .draw   = -1,
                .status = INIT
                });

        playResponse.subgameTriggered = true;
        playResponse.ended            = false;
        playResponse.empty            = true;
        // playResponse.refWinAmount     = 0;
        playResponse.action           = "choice";

        if (mode != MODE_FEATURE_BUY5)
            return;
    }

    if (mode < MODE_FEATURE_BUY3){
        numSC = runBaseSpin();
    }

    if (!playResponse.maxWinTriggered && 
            ((numSC >= REEL_COUNT) ||
             (mode == MODE_FEATURE_BUY5))
       ) {
        runFreeSpins ();
    }

    return;
}

void Round::next (bool gamble) {
    FsStatus fsStatus;
    FsStatus& prevFsStatus = playResponse.fsStatus.back();;

    if (prevFsStatus.level < MAX_FS_LEVEL && gamble) {
        auto [succeeded, prob] = tryGamble (prevFsStatus.level);
        LOG ("[next] gamble has " ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", succeeded ? "succeeded":"failed");

        prevFsStatus.draw = prob;

        if (succeeded) {
            prevFsStatus.status = SUCCEEDED;
            fsStatus.level      = prevFsStatus.level + 1;
            fsStatus.status     = INIT;
            fsStatus.draw       = -1;
            playResponse.fsStatus.push_back(fsStatus);

            if (fsStatus.level >= MAX_FS_LEVEL) {
                LOG("[next] collecting since \"fs level\"(%d) >= MAX_FSLVL(%d)\n", fsStatus.level, MAX_FS_LEVEL);
                return collect(playResponse);
            }

            return;
        }

        // Else gamble failed
        prevFsStatus.status = FAILED;
        playResponse.ended  = true;
        return;
    }

    // Else collect 
    return collect(playResponse);
}

void Round::collect (PlayResponse& playResponse) {
    FsStatus& fsStatus = playResponse.fsStatus.back();;
    int numSC = fsStatus.level + 2;
    return runFreeSpins ();
}

std::vector<std::string> getActiveFeatures (const std::vector<FsFeature>& features) {
    std::vector<std::string> activeFeats;

    for (const auto& af: features) {
        auto afStr = FS_FEATURE_STR (af);
        activeFeats.push_back (afStr);
    }

    return activeFeats;
}

ReelSetWeightSet Round::selectReelsWeightSet (const int mode) {
    ReelSetWeightSet selectedSet = ReelSetWeightSet::None;
    double prob;
    const double randVal = rng.getDouble();

    switch (mode) {
        case MODE_NORMAL: prob   = ProbReelWeightRegularSetA; break;
        case MODE_ENHANCED: prob = ProbReelWeightBonusBoostSetA; break;
        case MODE_SB: prob       = ProbReelWeightSuperBoostSetA; break;
        default: throw_error ("invalid mode for reel-set-weight-set selection, mode = ", mode);
    }

    selectedSet = (randVal < prob ? ReelSetWeightSet::A : ReelSetWeightSet::B);
    if (selectedSet == ReelSetWeightSet::None)
        throw_error ("no reel-set-weight-set selected for mode = ", mode);

    return selectedSet;
}


int Round::runBaseSpin() {
    FsStatus fsStatus;
    auto& baseSpin = playResponse.baseSpin;

    // pick a reel-set prob/weight set, and use this during the BASEGAME(SPIN)
    ReelSetWeightSet set = selectReelsWeightSet (mode);
    payWindow.selectReelset (true, false, mode, false, set);

    baseSpin.stops   = payWindow.spin (true, mode, false);
    baseSpin.reelSet = payWindow.getReelSetName();

    payWindow.drawCsSymbols (baseSpin.coinSyms, playResponse.refBetBase);

    payWindow.snapshot (baseSpin.stoppedGrid);
    payWindow.snapshot (baseSpin.symbols);

    auto [refWAmt, maxWinTrig] = payWindow.getWinnings (
            baseSpin.winnings,
            playResponse.refBetBase,
            baseSpin.refWinsSoFar
            );

    baseSpin.numSC = payWindow.symbolCount (SC);

    const bool shf = payWindow.scatterHook (baseSpin, mode);
    payWindow.snapshot (baseSpin.scatterHookSymbols); // always snap needed as to avoid seg-fault

    baseSpin.refWinAmount         = refWAmt;
    baseSpin.refWinsSoFar         = refWAmt;
    baseSpin.refBaseWinAmount     = refWAmt;
    baseSpin.maxWinTriggered      = maxWinTrig;

    if (baseSpin.numSC == 2 && !shf && payWindow.shouldReSpin())
        runReSpins (baseSpin);


    playResponse.subgameTriggered = false;
    playResponse.maxWinTriggered  = baseSpin.maxWinTriggered;
    playResponse.ended            = true;
    playResponse.empty            = false;

    int scCount = baseSpin.numSC;
    if (baseSpin.reSpins.size() > 0 && baseSpin.reSpins.back().numSC > scCount)
        scCount = baseSpin.reSpins.back().numSC;

    if (scCount >= 3 && scCount <= REEL_COUNT) {
        fsStatus.level                = numSCToFsLevel (scCount);
        fsStatus.status               = INIT;
        fsStatus.draw                 = -1.0;
        playResponse.subgameTriggered = true;
        playResponse.ended            = false;
        playResponse.fsStatus.push_back (fsStatus);
    }

    return baseSpin.numSC;
}

void Round::reSpin (std::vector<std::pair<int, int>>& nudgingReelsMeta, const std::string& bgReelSet, Spin& spin, const int64_t refWinsSoFar) {
    spin.reelSet = payWindow.getReelSetName();
    spin.stops = payWindow.spin (true, mode, false);

    payWindow.snapshot(spin.stoppedGrid);
    payWindow.snapshot(spin.scatterHookSymbols);

    // apply nudging reels stops
    payWindow.applyNudgingReels (nudgingReelsMeta, bgReelSet, mode);

    // update the stops after nudging reels
    for (const auto& [i, stop]: nudgingReelsMeta)
        spin.stops[i] = stop;

    payWindow.drawCsSymbols (spin.coinSyms, playResponse.refBetBase);
    spin.numSC = payWindow.symbolCount (SC);
    if (spin.numSC < 2)
        throw_error ("no. of scatters in re-spin less than 2, not possible though, #SC = ", spin.numSC);

    payWindow.snapshot(spin.symbols);
    auto [refWAmt, maxWinTrig] = payWindow.getWinnings (spin.winnings, playResponse.refBetBase, refWinsSoFar);

    spin.refBaseWinAmount = refWAmt;
    spin.refWinAmount     = refWAmt;
    spin.refWinsSoFar     = refWAmt + refWinsSoFar;
    spin.maxWinTriggered  = maxWinTrig;

    playResponse.maxWinTriggered  = spin.maxWinTriggered;
}

void Round::runReSpins (BaseSpin& baseSpin) {
    LOG ("Playing Re-Spins\n");

    std::vector<std::pair<int, int>> nudgingReelsMeta;
    for (int i = 0; i < REEL_COUNT; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            if (baseSpin.symbols[i][j] == SC){
                if (j == GRID_HEIGHT-1)
                    throw_error ("SC symbols at the bottom, shouldn't be in re-spins");

                nudgingReelsMeta.push_back ({ i, baseSpin.stops[i] });
                break;
            }
        }
    }

    if (nudgingReelsMeta.size() != 2)
        throw_error ("re-spins should happen with exactly 2 SC, found ", nudgingReelsMeta.size());

    payWindow.selectReelSetByNameAndWeights (mode, baseSpin.reelSet);

    // run first re-spin
    Spin reSpin1;
    reSpin (nudgingReelsMeta, baseSpin.reelSet, reSpin1, baseSpin.refWinsSoFar);
    baseSpin.reSpins.push_back (reSpin1);

    if (reSpin1.numSC >= 3)
        return;

    if (reSpin1.numSC < 3 && payWindow.shouldReSpin()) {
        Spin reSpin2;
        reSpin2.index = 1;
        reSpin (nudgingReelsMeta, baseSpin.reelSet, reSpin2, reSpin1.refWinsSoFar);

        baseSpin.reSpins.push_back (reSpin2);
        return;
    }
}

CoinSet Round::chooseCoinSet (const int fsLevel) {
    const double randVal = rng.getDouble ();
    const double prob    = COIN_WEIGHT_SET_PROB_A [fsLevel-1];
    return (randVal < prob ? CoinSet::A : CoinSet::B);
}

void Round::selectFeatures (const int numFeatures, std::map<FsFeature, bool>& features) {
    if (numFeatures <= 0)
        return;

    std::vector<int> featIdxs;
    for (int i = 0; i < FS_FEATURES.size(); i++)
        featIdxs.push_back(i);

    int i = 0;
    while (i < numFeatures) {
        const int randIdx       = rng.getScaled (featIdxs.size());
        const int featIdx       = featIdxs [randIdx];
        const FsFeature feature = FS_FEATURES [featIdx];

        if (features.find(feature) == features.end()) {
            features.insert ({ feature, true});
            i++;
            featIdxs.erase (featIdxs.begin() + randIdx);
        } else {
            throw_error ("some problem with feature selection, duplication detected");
        }
    }
}

int64_t getCoinSymbolWins (const CoinSym_Vec& coinSymbols, const int wsCount) {
    int64_t totalWin = 0;

    for (auto cs: coinSymbols) {
        totalWin += cs.refValue;
    }

    return totalWin * wsCount;
}

WsCollection getWsColInfoByWildSoFar (const int wsSoFar) {
    if (wsSoFar < 0)
        throw_error ("invalid no of wilds collected = ", wsSoFar);

    int level = 1;

    if (wsSoFar < 4) {
        level = 1;
    } else if (wsSoFar < 8) {
        level = 2;
    } else if (wsSoFar < 12) {
        level = 3;
    } else {
        level = 4;
    }

    return WS_COLLECTION_SPECS [level-1];
}

WsCollection getWsColInfoByLevel (const int level) {
    return WS_COLLECTION_SPECS [level-1];
}

void validateFsPersistentValues(const FsPersistentValues& fpv) {
    std::vector<std::string> errors;

    if (fpv.fsLevel <= 0)
        errors.push_back("fsLevel=" + std::to_string(fpv.fsLevel));

    if (fpv.numSpins <= 0)
        errors.push_back("numSpins=" + std::to_string(fpv.numSpins));

    if (fpv.coinSet == CoinSet::None)
        errors.push_back("coinSet=None");

    if (fpv.numFeatures < 0)
        errors.push_back("numFeatures=" + std::to_string(fpv.numFeatures));

    if (fpv.features.size() < 0)
        errors.push_back("features.size() < 0");

    if (fpv.features.size() != fpv.numFeatures)
        errors.push_back("features.size()=" + std::to_string(fpv.features.size()) + " != numFeatures=" + std::to_string(fpv.numFeatures));

    if (!errors.empty()) {
        std::string combined = "Invalid FsPersistentValues: ";
        for (size_t i = 0; i < errors.size(); ++i) {
            combined += errors[i];
            if (i < errors.size() - 1)
                combined += ", ";
        }

        throw_error(combined);
    }
}

FsPersistentValues Round::preFsSetup (const FsStatus& fsStatus) {
    FsPersistentValues fpv;

    fpv.fsLevel     = fsStatus.level;
    fpv.numSpins    = NUM_AWARDED_FREE_SPINS [fpv.fsLevel-1];
    fpv.coinSet     = chooseCoinSet (fpv.fsLevel);
    fpv.numFeatures = rng.getWeightedIndex (NUM_FS_FEATURES_WEIGHT.data(), NUM_FS_FEATURES);
    fpv.numFeatures = 0;

    selectFeatures (fpv.numFeatures, fpv.features);
    for (auto& ftr: fpv.features) {
        if (ftr.second)
            playResponse.activeFeatures.push_back(ftr.first);
    }

    WsCollection initialWSSpecs = getWsColInfoByLevel (START_WS_LEVEL);
    fpv.wsCollected.soFar           = initialWSSpecs.wsReq;
    fpv.wsCollected.activeLevel     = initialWSSpecs.colLevel;
    fpv.wsCollected.triggeredLevel  = initialWSSpecs.colLevel;
    fpv.wsCollected.coinMulti       = initialWSSpecs.coinMultiplier;

    fpv.moreCoins    = (fpv.features.find (FsFeature::MORE_COINS)       != fpv.features.end());
    fpv.extraWilds   = (fpv.features.find (FsFeature::MORE_WILDS)       != fpv.features.end());
    fpv.startOnLvl2  = (fpv.features.find (FsFeature::START_ON_LEVEL_2) != fpv.features.end());
    fpv.extra2Spins  = (fpv.features.find (FsFeature::EXTRA_2_SPINS)    != fpv.features.end());
    fpv.removeLowest = (fpv.features.find (FsFeature::REMOVE_LOWEST)    != fpv.features.end());

    if (fpv.startOnLvl2) {
        auto wsColInfo   = getWsColInfoByLevel (2);
        fpv.wsCollected.soFar          = wsColInfo.wsReq;
        fpv.wsCollected.activeLevel    = wsColInfo.colLevel;
        fpv.wsCollected.triggeredLevel = wsColInfo.colLevel;
        fpv.wsCollected.coinMulti      = wsColInfo.coinMultiplier;
    }

    if (fpv.extra2Spins)
        fpv.numSpins += 2;

    fpv.extraSpin = false;

    validateFsPersistentValues (fpv);
    return fpv;
}

void Round::fsBaseRun (FsPersistentValues& persistent, FreeSpin& freeSpin, bool forced, bool moreCoins, bool& gtdWildSpecial) {
    freeSpin.reelSet = payWindow.getReelSetName();
    freeSpin.stops   = payWindow.spin (false, mode, false, forced, moreCoins, freeSpin.index);

    payWindow.replaceCR (
            persistent.extraWilds,
            persistent.wsCollected,
            playResponse.refBetBase,
            freeSpin.coinSyms,
            persistent.coinSet,
            freeSpin.wildSyms,
            persistent.firstExtraSpin,
            gtdWildSpecial,
            persistent.removeLowest,
            persistent.extraSpin
            );

    payWindow.snapshot (freeSpin.stoppedGrid);
    payWindow.snapshot (freeSpin.scatterHookSymbols);
    gtdWildSpecial = false; // reset once done with it.
}

void Round::updateFsLevelInfo (FsPersistentValues& persistent, FreeSpin& freeSpin, int& currLevelSpinsLeft) {
    const WsCollection updatedWsColInfo = getWsColInfoByWildSoFar (persistent.wsCollected.soFar);
    persistent.wsCollected.triggeredLevel = updatedWsColInfo.colLevel;

    freeSpin.newWsLanded          = payWindow.symbolCount (WS);
    persistent.wsCollected.soFar += freeSpin.newWsLanded;
    freeSpin.wsCollectedSoFar     = persistent.wsCollected.soFar;
    freeSpin.level                = persistent.wsCollected.activeLevel;
    freeSpin.coinMultiplier       = persistent.wsCollected.coinMulti;

    // updates about the current/active level, used in coin-multi, coin-symbol placements
    if (currLevelSpinsLeft == 0 && persistent.wsCollected.triggeredLevel > persistent.wsCollected.activeLevel) {
        persistent.wsCollected.activeLevel     += 1;
        auto updatedActiveLvlInfo = getWsColInfoByLevel (persistent.wsCollected.activeLevel);
        persistent.wsCollected.coinMulti        = updatedWsColInfo.coinMultiplier;
        currLevelSpinsLeft                     += updatedWsColInfo.extraSpins;
        currLevelSpinsLeft                     += (persistent.extra2Spins ? 2 : 0);
        persistent.numSpins                    += currLevelSpinsLeft;
        freeSpin.extraSpins                     = currLevelSpinsLeft;
    }
}

void Round::evalFsWinnings (const FsPersistentValues& persistent, FreeSpin& freeSpin, int64_t& refWinsSoFar, bool& maxWinTriggered) {
    payWindow.snapshot (freeSpin.symbols);
    auto [refWinAmt, maxWinTrig] = payWindow.getWinnings (
            freeSpin.winnings,
            playResponse.refBetBase,
            refWinsSoFar
            );

    refWinsSoFar             += refWinAmt;
    freeSpin.refBaseWinAmount = refWinAmt;

    const int64_t MAX_WIN_AMOUNT = MAX_WIN_CAP * playResponse.refBetBase;

    if (!maxWinTrig) {
        int64_t refCSWins = getCoinSymbolWins (freeSpin.coinSyms, freeSpin.newWsLanded) * freeSpin.coinMultiplier;
        maxWinTrig        = (refWinsSoFar + refCSWins) >= MAX_WIN_AMOUNT;

        // TODO: somehow max-win error occurs, do check it
        if (maxWinTrig) {
            refCSWins = (MAX_WIN_AMOUNT - refWinsSoFar);
            LOG ("Max Win reached using COIN SYMBOLS, %ld\n", refCSWins);
        }

        freeSpin.refCoinSymbolWins = refCSWins;
    }

    refWinsSoFar                += freeSpin.refCoinSymbolWins;
    freeSpin.refWinAmount        = freeSpin.refBaseWinAmount + freeSpin.refCoinSymbolWins;
    freeSpin.refWinsSoFar        = refWinsSoFar;
    freeSpin.maxWinTriggered     = maxWinTrig;
    maxWinTriggered              = maxWinTrig;
}

void Round::preFsValidation (const FsStatus& fsStatus) {
    if (playResponse.fsStatus.size() <= 0)
        throw_error("fs status is empty but entered into free spins");

    if (fsStatus.status == FAILED)
        throw_error("entered into freespins without success/failure");

    if (fsStatus.level < 1 || fsStatus.level > MAX_FS_LEVEL)
        throw_error ("fsStatus.level = ", fsStatus.level, " is out of bounds, in free-spins");
}

void Round::updateForcedWinFs (FsPersistentValues& persistent, const int64_t refWinsSoFar, bool& forced, const FreeSpin& fs) {
    if (fs.index == persistent.numSpins - 1 && persistent.wsCollected.activeLevel == persistent.wsCollected.triggeredLevel && refWinsSoFar == 0)
        forced = true;
    else
        forced = false;


    forced = false;
}

void Round::guaranteedWildsSpecial (FsPersistentValues& persistent, const int64_t refWinsSoFar, const FreeSpin& fs, bool& gtdWildSpecial) {
    const bool lastThree = (fs.index >= persistent.numSpins - 3 && fs.index < persistent.numSpins);

    if (
            refWinsSoFar == 0
            && persistent.wsCollected.activeLevel == persistent.wsCollected.triggeredLevel
            && lastThree
       ) {
        gtdWildSpecial = true;
    } else {
        gtdWildSpecial = false;
    }
}

void Round::runFreeSpins () {
    FsStatus& fsStatus = playResponse.fsStatus.back();
    preFsValidation (fsStatus);

    if (
            playResponse.baseSpin.reSpins.size() > 0 &&
            (playResponse.baseSpin.refWinsSoFar > playResponse.baseSpin.reSpins.back().refWinsSoFar)
       )
        throw_error (
                "shouldn't happen: base-wsf = ",
                playResponse.baseSpin.refWinsSoFar,
                ", re-spin-wsf =  ",
                playResponse.baseSpin.reSpins.back().refWinsSoFar
                );

    const auto refBaseWinAmount  = playResponse.baseSpin.reSpins.size() > 0 ?
        playResponse.baseSpin.reSpins.back().refWinsSoFar :
        playResponse.baseSpin.refWinsSoFar;

    int64_t refWinsSoFar = refBaseWinAmount;
    bool maxWinTriggered = false;
    bool forced          = false;
    bool lastFour        = false;
    bool gtdWildSpecial  = false; // this resets multiple times during the whole fs session

    // Pre FS Setup
    FsPersistentValues persistent = preFsSetup (fsStatus);
    payWindow.selectReelset (false, persistent.moreCoins, mode, forced);
    int currLevelSpinsLeft = persistent.numSpins;

    LOG (
            "\n%s%sRunning Free Spins, Total No. of Spins = %d, Starting with WS collection = %d%s\n",
            ANSI_BOLD,
            ANSI_COLOR_RED,
            persistent.numSpins,
            persistent.wsCollected.soFar,
            ANSI_COLOR_RESET
        );

    for (int i = 0; i < persistent.numSpins; i++) {
        currLevelSpinsLeft --;

        FreeSpin freeSpin;
        freeSpin.index   = i;

        updateForcedWinFs (persistent, refWinsSoFar, forced, freeSpin);
        fsBaseRun (persistent, freeSpin, forced, persistent.moreCoins, gtdWildSpecial);
        updateFsLevelInfo (persistent, freeSpin, currLevelSpinsLeft);
        evalFsWinnings (persistent, freeSpin, refWinsSoFar, maxWinTriggered);
        guaranteedWildsSpecial (persistent, refWinsSoFar, freeSpin, gtdWildSpecial);

        playResponse.freeSpins.push_back (freeSpin);
        playResponse.maxWinTriggered = maxWinTriggered;
        if (maxWinTriggered)
            break;
    }

    nextExtraSpin (persistent);

    fsStatus.status    = COLLECTED;
    playResponse.ended = true;
    playResponse.empty = false;

    return;
}

FsPersistentValues Round::getExtraSpinsPersistent (const PlayResponse& prevResp) {
    FsPersistentValues fpv;

    const auto prevLastFS         = prevResp.freeSpins.back();
    const auto wildsCollectedPrev = prevLastFS.wsCollectedSoFar;
    const auto activeFeatStrs     = getActiveFeatures (prevResp.activeFeatures);
    const auto esi                = getExtraSpinInfo (activeFeatStrs, (wildsCollectedPrev >= 12 ? 12: wildsCollectedPrev));

    const double randVal = rng.getDouble ();
    fpv.coinSet          = (randVal < esi.coinSetAProb ? CoinSet::A : CoinSet::B);
    fpv.fsLevel          = -1;
    fpv.numSpins         = 1;
    fpv.numFeatures      = activeFeatStrs.size();
    fpv.moreCoins        = esi.moreCoins;
    fpv.removeLowest     = esi.removeLowest;
    fpv.extra2Spins      = esi.extra2Spins;
    fpv.extraWilds       = esi.extraWilds;
    fpv.extraSpin        = true;
    fpv.firstExtraSpin   = true;

    for (auto& x: playResponse.activeFeatures)
        fpv.features.insert({ x, true });

    fpv.wsCollected.soFar          = prevLastFS.wsCollectedSoFar;
    fpv.wsCollected.activeLevel    = prevLastFS.level;
    fpv.wsCollected.triggeredLevel = prevLastFS.level;
    fpv.wsCollected.coinMulti      = prevLastFS.coinMultiplier;

    return fpv;
}

void Round::runExtraSpins (const PlayResponse& prevResp) {
    if (prevResp.freeSpins.empty())
        throw_error ("playing extra-spins before playing free-spins");

    playResponse.extraSpin      = true;
    playResponse.activeFeatures = prevResp.activeFeatures;
    playResponse.action         = "extra-spin";

    FsPersistentValues persistent  = getExtraSpinsPersistent (prevResp);
    const int64_t refBaseWinAmount = 0;
    int64_t refWinsSoFar           = refBaseWinAmount;
    bool maxWinTriggered           = false;
    bool forced                    = false;
    bool lastFour                  = false;
    int currLevelSpinsLeft         = persistent.numSpins;
    bool gtdWildSpecial            = false;

    LOG (
            "\n%s%sRunning Extra Spin(Free Spins), Total No. of Spins = %d, Starting with WS collection = %d%s\n",
            ANSI_BOLD,
            ANSI_COLOR_RED,
            persistent.numSpins,
            persistent.wsCollected.soFar,
            ANSI_COLOR_RESET
        );

    payWindow.selectReelset (false, persistent.moreCoins, mode, false);
    for (int i = 0; i < persistent.numSpins; i++) {
        currLevelSpinsLeft --;

        FreeSpin freeSpin;
        freeSpin.index   = i;

        if (!persistent.firstExtraSpin) {
            //TODO: guaranteed wild special rule
        }

        fsBaseRun (persistent, freeSpin, false, persistent.moreCoins, gtdWildSpecial);
        updateFsLevelInfo (persistent, freeSpin, currLevelSpinsLeft);
        evalFsWinnings (persistent, freeSpin, refWinsSoFar, maxWinTriggered);

        playResponse.freeSpins.push_back (freeSpin);
        playResponse.maxWinTriggered = maxWinTriggered;
        if (maxWinTriggered)
            break;

        persistent.firstExtraSpin = false;
    }

    nextExtraSpin (persistent);
    playResponse.ended = true;
    playResponse.subgameTriggered = false;
}

void Round::nextExtraSpin (const FsPersistentValues& persistent) {
    const auto wsColSoFar = (persistent.wsCollected.soFar <= 12 ? persistent.wsCollected.soFar: 12);
    std::vector<std::string> activeFeatures = getActiveFeatures (playResponse.activeFeatures);

    ExtraSpinInfo esi = getExtraSpinInfo (activeFeatures, wsColSoFar);
    if (esi.costX < 0.0 || esi.featureMode < 0.0 || esi.id < 0)
        throw_error ("couldn't find a valid extra-spin combination, wildsCollected = ", wsColSoFar, ", totalFeatures = ", activeFeatures.size());

    playResponse.buyExtraSpin.extraSpin   = true;
    playResponse.buyExtraSpin.costX       = esi.costX;
    playResponse.buyExtraSpin.featureMode = esi.featureMode;
    playResponse.buyExtraSpin.expectedBet = playResponse.refBetBase * esi.costX;
    playResponse.buyExtraSpin.id          = esi.id;
}

std::pair<bool, float> Round::tryGamble(const int prevLevel) {
    float successProb = LADDER_WEIGHTS [prevLevel - 1];
    float prob        = rng.getDouble();
    return std::make_pair(prob < successProb, prob);
}

void Round::clear () {
    payWindow.clear();
}
