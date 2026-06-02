#pragma once

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include "paywindow.h"
#include "./data-defs.h"
#include "../utils/error.h"
#include "rng.h"

struct WsCollection {
    int colLevel       = 0;
    int wsReq          = 0;
    int coinMultiplier = 0;
    int extraSpins     = 0;
};

struct FsPersistentValues {
    int fsLevel       = -1;
    int numSpins      = -1;
    CoinSet coinSet   = CoinSet::None;
    int numFeatures   = -1;
    bool moreCoins    = false;
    bool extraWilds   = false;
    bool startOnLvl2  = false;
    bool extra2Spins  = false;
    bool removeLowest = false;

    std::map<FsFeature, bool> features;
    WildCollection wsCollected;
    bool extraSpin       = false;
    bool firstExtraSpin  = false;
};

class Round {
    public:
        Round (RngLib& rng, PlayResponse& playResponse, PayWindow& payWindow);
        void play ();
        void next (bool gamble);
        void runExtraSpins (const PlayResponse& prevResp);

    private:
        unsigned int spinCount = 0;
        std::vector<std::vector<Symbol>> takeGridSnapshot();

        // free spin methods
        CoinSet chooseCoinSet (const int fsLevel);
        void selectFeatures (const int numFeatures, std::map<FsFeature, bool>& features);

        // primitive methods
        virtual int runBaseSpin();
        void runReSpins (BaseSpin& baseSpin);
        void reSpin (std::vector<std::pair<int, int>>& nudgingReelsMeta, const std::string& bgReelSet, Spin& spin, const int64_t refWinsSoFar);

        void preFsValidation (const FsStatus& fsStatus);
        FsPersistentValues preFsSetup (const FsStatus& fsStatus);
        void fsBaseRun (FsPersistentValues& persistent, FreeSpin& freeSpin, bool forced, bool moreCoins, bool& gtdWildSpecial);
        void updateFsLevelInfo (FsPersistentValues& persistent, FreeSpin& freeSpin, int& currLevelSpinsLeft);
        void evalFsWinnings (const FsPersistentValues& persistent, FreeSpin& freeSpin, int64_t& refWinsSoFar, bool& maxWinTriggered);
        void updateForcedWinFs (FsPersistentValues& persistent, const int64_t refWinsSoFar, bool& forced, const FreeSpin& fs);
        void guaranteedWildsSpecial (FsPersistentValues& persistent, const int64_t refWinsSoFar, const FreeSpin& fs, bool& gtdWildSpecial);
        void runFreeSpins();

        FsPersistentValues getExtraSpinsPersistent (const PlayResponse& prevResp);
        void nextExtraSpin (const FsPersistentValues& persistent);

        std::pair<bool, float> tryGamble(const int prevLevel);
        void collect(PlayResponse& playResponse);
        void clear ();


        ReelSetWeightSet selectReelsWeightSet (const int mode);

    protected:
        RngLib& rng;
        PlayResponse& playResponse;

        PayWindow& payWindow;
        const int mode;
};

inline constexpr int MAX_FS_LEVEL = 5;
inline constexpr int MAX_GAMBLE_LEVEL = MAX_FS_LEVEL - 1;
inline constexpr int NUM_FS_FEATURES = MAX_FS_LEVEL + 1;
constexpr inline int START_WS_LEVEL  = 1;

inline constexpr std::array<WsCollection, MAX_FS_LEVEL-1> WS_COLLECTION_SPECS = {{
    { 1,  0,  1, 10 },
    { 2,  4,  2, 10 },
    { 3,  8,  3, 10 },
    { 4, 12, 10, 10 },
}};

#if defined(VARIANT_96)
inline constexpr double ProbReelWeightRegularSetA    = 0.2723;
inline constexpr double ProbReelWeightBonusBoostSetA = 0.2401;
inline constexpr double ProbReelWeightSuperBoostSetA = 0.4412;
inline constexpr std::array<double, MAX_FS_LEVEL> COIN_WEIGHT_SET_PROB_A = { 0.3194, 0.2969, 0.2188, 0.3623, 0.3614, };

#elif defined(VARIANT_94)
inline constexpr double ProbReelWeightRegularSetA    = 0.4949;
inline constexpr double ProbReelWeightBonusBoostSetA = 0.3592;
inline constexpr double ProbReelWeightSuperBoostSetA = 0.5015;
inline constexpr std::array<double, MAX_FS_LEVEL> COIN_WEIGHT_SET_PROB_A = { 0.5177, 0.4994, 0.4258, 0.5681, 0.5686, };

#elif defined(VARIANT_92)
inline constexpr double ProbReelWeightRegularSetA    = 0.7282;
inline constexpr double ProbReelWeightBonusBoostSetA = 0.4838;
inline constexpr double ProbReelWeightSuperBoostSetA = 0.5645;
inline constexpr std::array<double, MAX_FS_LEVEL> COIN_WEIGHT_SET_PROB_A = { 0.716, 0.702, 0.6328, 0.7739, 0.7758, };

#else
#error "no variant selected"
#endif

inline constexpr std::array<double, MAX_GAMBLE_LEVEL> LADDER_WEIGHTS = {{
    7.0/12.0,
    24.0/35.0,
    7.0/9.0,
    9.0/11.0
}};

inline constexpr std::array<int, MAX_FS_LEVEL> NUM_AWARDED_FREE_SPINS = {
    /* Level = 1 */ 10,
    /* Level = 2 */ 15,
    /* Level = 3 */ 20,
    /* Level = 4 */ 25,
    /* Level = 5 */ 30
};

inline constexpr std::array<int, NUM_FS_FEATURES> NUM_FS_FEATURES_WEIGHT = {
    400, 582, 436,
    137, 39, 29,
};

inline constexpr std::array<FsFeature, NUM_FS_FEATURES - 1> FS_FEATURES = {{
    FsFeature::MORE_WILDS, FsFeature::START_ON_LEVEL_2,
    FsFeature::REMOVE_LOWEST, FsFeature::MORE_COINS,
    FsFeature::EXTRA_2_SPINS
}};

inline const std::string FS_FEATURE_STR (const FsFeature& feat) {
    std::string featStr;

    switch (feat) {
        case FsFeature::MORE_WILDS:       featStr = "MORE_WILDS"; break;
        case FsFeature::START_ON_LEVEL_2: featStr = "START_ON_LEVEL_2"; break;
        case FsFeature::REMOVE_LOWEST:    featStr = "REMOVE_LOWEST"; break;
        case FsFeature::MORE_COINS:       featStr = "MORE_COINS"; break;
        case FsFeature::EXTRA_2_SPINS:    featStr = "EXTRA_2_SPINS"; break;
        case FsFeature::NONE:             featStr = "NONE"; break;
        default: throw std::logic_error ("invalid feature mode");
    }

    return featStr;
}

inline const FsFeature FS_FEATURE_FROM_STR (const std::string& featStr) {
    FsFeature feat;

    if (featStr == "MORE_WILDS") {
        feat = FsFeature::MORE_WILDS;
    } else if (featStr == "START_ON_LEVEL_2") {
        feat = FsFeature::START_ON_LEVEL_2;
    } else if (featStr == "REMOVE_LOWEST") {
        feat = FsFeature::REMOVE_LOWEST;
    } else if (featStr == "MORE_COINS") {
        feat = FsFeature::MORE_COINS;
    } else if (featStr == "EXTRA_2_SPINS") {
        feat = FsFeature::EXTRA_2_SPINS;
    } else if (featStr == "NONE") {
        feat = FsFeature::NONE;
    } else {
        throw_error (
                "invalid feature mode = ",
                featStr,
                ", must be one of these: [``MORE_WILDS``, ``START_ON_LEVEL_2``, ``REMOVE_LOWEST``, ``MORE_COINS``, ``EXTRA_2_SPINS`]"
        );
    }

    return feat;
}

