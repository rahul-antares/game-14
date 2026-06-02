#ifndef CHEAT_DEFS_H
#define CHEAT_DEFS_H

#include <cstdint>
#include <string>
#include <array>
#include "constants.h"
#include "napi.h"

struct Cheats {
    // Trivial
    double                      minWin              = -1;
    double                      maxWin              = -1;
    std::string                 winSymbol;
    int                         winSymbolCount      = -1;
    std::string                 excludeSymbol;
    int                         paylineWin          = -1;
    std::array<int, REEL_COUNT> stops;
    int                         reelset             = -1;

    // Game Specific
    int64_t                                 coinValue                      = -1;
    int                                     numCoins                       = -1;
    int                                     respin                         = -1;
    int                                     topRowRespin                   = -1;
    int                                     scatterHook                    = -1;
    int                                     freespins                      = -1;
    int                                     numScatters                    = -1;
    int                                     numFeatures                    = -1;
    std::vector<std::string>                features;
    int                                     minCollectionLevel             = -1;
    int                                     maxCollectionLevel             = -1;
    double                                  minFSWin                       = -1;
    double                                  maxFSWin                       = -1;
    int                                     fsDoubleWild                   = -1;
    int64_t                                 fsCoinValue                    = -1;
    int64_t                                 fsCoinValueEveryLevel          = -1;
    int64_t                                 fsCoinValueCollected           = -1;
    int64_t                                 fsCoinValueCollectedEveryLevel = -1;
    std::vector<std::array<int,REEL_COUNT>> fsStops;
    int                                     extraSpinTrigger               = -1;
    int                                     winCap                         = -1;
    int                                     gambleSuccessLevel             = -1;
};

struct Cheat {
    std::string name;
    std::string type;
    std::string description;
    std::string implMethod;
    bool implemented;

    Cheat (std::string name, std::string type, std::string description, std::string implMethod, bool implemented):
        name(name), type(type), description(description), implMethod(implMethod),  implemented(implemented)
    {};
};

const std::vector<Cheat> CheatsAll = {
    {
        "minWin",
        "double",
        "the minimum base game win expressed as a bet multiplier",
        "random draws until fulfilled",
        true
    },
    {
        "maxWin",
        "double",
        "the maximum base game win expressed as a bet multiplier",
        "random draws until fulfilled",
        true
    },
    {
        "winSymbol",
        "string",
        "the symbol type that generates a win",
        "random draws until fulfilled",
        true
    },
    {
        "winSymbolCount",
        "int",
        "the number of above given symbol",
        "random draws until fulfilled",
        true
    },
    {
        "excludeSymbol",
        "string",
        "symbol to be excluded from win",
        "random draws until fulfilled",
        true
    },
    {
        "paylineWin",
        "int",
        "any win on the given pay line (0 - 9) in the base spin",
        "random draws until fulfilled",
        true
    },
    {
        "stops",
        "array<int>",
        "the stop positions for the five reels",
        "forced outcome",
        true
    },
    {
        "reelset",
        "int",
        "the specified reelset should be used",
        "forced outcome",
        true
    },
    {
        "coinValue",
        "int",
        "spin should contain a CS symbol with the specified coin value. random draws until a round containing a coin is there",
        "then force the value for one random coin to the selected value",
        true
    },
    {
        "numCoins",
        "int",
        "number of coin symbols landing in base game",
        "random draws until fulfilled",
        true
    },
    {
        "respin",
        "boolean",
        "guarantees landing two SC symbols on the top two rows (and none on the bottom)",
        "random draws until fulfilled",
        true
    },
    {
        "topRowRespin",
        "boolean",
        "guarantees landing two SC symbols on the top row (thereby possibly getting two re-spins)",
        "random draws until fulfilled",
        true
    },
    {
        "scatterHook",
        "boolean",
        "guarantees landing two SC symbols and then the scatter hook appearing guaranteeing using the hook if possible",
        "random draws until an eligible scenario occurs",
        true
    },
    {
        "freespins",
        "boolean",
        "guarantees landing at least 3 SC during a spin (may include a respin; the \"respin\" flag can be set to false to avoid)",
        "random draws until fulfilled",
        true
    },
    {
        "numScatters",
        "int",
        "number of SC landing during a spin",
        "random draws until fulfilled",
        true
    },
    {
        "numFeatures",
        "int",
        "number of features being picked before free spins before the END PICK SEQUENCE is picked (0 - 5)",
        "forced outcome",
        true
    },
    {
        "features",
        "array<string>",
        "specific features being picked in the order given e.g. [``MORE_WILDS``, ``START_ON_LEVEL_2``, ``REMOVE_LOWEST``, ``MORE_COINS``, ``EXTRA_2_SPINS`]",
        "forced outcome",
        true
    },
    {
        "minCollectionLevel",
        "int",
        "free spin should reach the specified collection level or higher (1 - 4)",
        "random draws of the free spins function until fulfilled",
        true
    },
    {
        "maxCollectionLevel",
        "int",
        "free spins should not reach a collection level above the set one (1 - 4)",
        "random draws of the free spins function until fulfilled",
        true
    },
    {
        "minFSWin",
        "double",
        "the minimum win in free spins expressed as a bet multiplier",
        "random draws of the free spins function until fulfilled",
        true
    },
    {
        "maxFSWin",
        "double",
        "the maximum win in free spins expressed as a bet multiplier",
        "random draws of the free spins function until fulfilled",
        true
    },
    {
        "fsDoubleWild",
        "boolean",
        "contains at least one spin which lands two wilds",
        "random draws of the free spins function until fulfilled",
        true
    },
    {
        "fsCoinValue",
        "int",
        "at least one free spin should contain a CS symbol with the specified coin value.",
        "forced outcome",
        true
    },
    {
        "fsCoinValueEveryLevel",
        "int",
        "the first spin containing a CS symbol on each level should have a CS symbol with the specified value.",
        "forced outcome",
        true
    },
    {
        "fsCoinValueCollected",
        "int",
        "at least one free spin should contain a CS symbol with the specified coin value as well as at least one WS.",
        "forced outcome",
        true
    },
    {
        "fsCoinValueCollectedEveryLevel",
        "int",
        "the first spin on each level should have a CS symbol with the specified value as well as at least one WS.",
        "forced outcome",
        true
    },
    {
        "fsStops",
        "array<array<int>>",
        "the stop positions for the five reels in the first free spin the second and so on. After the array runs out (and there still are spins left to be played) it should revert to random stops.",
        "forced outcome",
        true
    },
    {
        "extraSpinRetrigger",
        "boolean",
        "(only for extra spin) guarantees landing enough WS to retrigger the next level random draws of the first spin until enough CR symbols land to be converted to WS then force the required number of CR to WS to complete the collection. IMPORTANT: to not risk creating infinite loops",
        "this flag should be ignored when the player is MORE THAN 2 collection units away from the next level",
        false
    },
    {
        "winCap",
        "boolean",
        "the round reaches the wincap random draws of base game until a free spin trigger occurs",
        "then random draws of the free spins function until fulfilled",
        true
    },
    {
        "gambleSuccessLevel",
        "int",
        "as long as the player tries to gamble it succeeds if the target level is on or below the selected level and fails when trying to gamble to any level above",
        "forced outcome",
        true
    }
};

extern Cheats parseCheats(const Napi::Env env, const Napi::Value c);

#endif // CHEAT_DEFS_H

