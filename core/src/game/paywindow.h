#pragma once
#include <cstdint>
#include <map>
#include "rng.h"
#include "./constants.h"
#include "./data-defs.h"
#include "./reelset.h"
#include "symbols.h"

struct WinSym {
    int count = 0;
    Coordinate_Vec coordinates;

    WinSym(): count(0), coordinates({}) {}
};

struct MultiWeight {
    Symbol symbol;
    int    weight;
};

enum class CoinSet {
    A = 0,
    B,
    None,
};

class PayWindow {
public:
    PayWindow(RngLib& rng, ReelSet& reelset);
    std::array<int, REEL_COUNT> spin (bool baseSpin, const int mode, bool reSpin, bool forced = false, bool moreCoins = false, int fsStopIdx = -1);

    void selectReelset(bool baseSpin, bool moreCoins, int mode, bool forced = false, ReelSetWeightSet rsWeightSet = ReelSetWeightSet::None);
    inline std::string getReelSetName() { return reelset.getSelected()->name; }
    inline void setSymbol(int i, int j, Symbol sym) { window[i][j] = sym; };
    inline Symbol getSymbol(int i, int j) { return window[i][j]; }


    std::pair<int64_t, bool> getWinnings(Winning_Vec& winnings, const int refBetBase, int64_t refWinsSoFar);
    int symbolCount(Symbol s);
    inline void setRoundRef(Round* round) {
        this->round = round;
        reelset.setRoundRef(round);
    };

    void drawCsSymbols (CoinSym_Vec& coinSymbols, const int64_t refBetBase);
    int getNumWSToPlace (const bool extraWilds, const int wsColLevel, const bool firstExtraSpin, const bool gtdWildSpecial = false);
    void placeCoinSymbolsForCR (
        CoinSym_Vec& coinSymbols,
        const int64_t& refBetBase,
        const WildCollection& wsCol,
        const CoinSet& coinSet,
        const bool removeLowest,
        const bool extraSpin
    );
    void replaceCR (
        const bool extraWilds,
        WildCollection& wsCol,
        const int64_t refBetBase,
        CoinSym_Vec& coinSymbols,
        const CoinSet& coinSet,
        SymCoords_Vec& wildSyms,
        const bool firstExtraSpin,
        const bool gtdWildSpecial,
        const bool removeLowest,
        const bool extraSpin
    );

    std::vector<std::pair<int, int>> scatterHookPositions (
        const std::vector<int>& nonScSymReels,
        const std::array<int, REEL_COUNT>& spinStops
    );

    bool scatterHook (Spin& _spin, const int mode);
    bool shouldReSpin ();
    void applyNudgingReels (std::vector<std::pair<int, int>>& nudgingReelsMeta, std::string nudgingReelSet, const int mode);

    void selectReelSetByNameAndWeights (const int mode, const std::string& baseGameReelset);
    int getCoinSymbolForCRRep (const int& wsColLevel, const CoinSet& coinSet, const bool removeLowest, const bool extraSpin);

    Grid snapshot();
    // More optimized version
    void snapshot (Grid& grid);
    void snapshot (GridV& grid);

    void clear();
    void printGrid(const char* message);

protected:
    RngLib&      rng;
    ReelSet&     reelset;
    Symbol       window[REEL_COUNT][GRID_HEIGHT];
    Round*       round;

private:
    void printWinSymsMap (std::map<Symbol, WinSym>& symMap);
    void printHighlightWinningSyms (std::map<Symbol, WinSym> symMap);
};

inline const std::vector<int> CoinSymbolsWeight = {
    38210, 43470, 13670, /* 2, 5, 10 */
     3540,  2010,   800, /* 15, 20, 25 */
      400,   200,   100, /* 50, 100, 200 */
       60,    37,    20, /* 500, 1666, 2500 */
        8                /* 5000 */
};

inline constexpr int MAX_WS_PLACEMENT_LIMIT = 2;

// [x][y] => x :: (WS-col-level, y = num-CR) => x = weight vector
inline const std::vector<std::vector<std::vector<int>>> CR_REPLACEMENT_WS_COUNT_NO_EXTRA_WILDS = {
    { /* level 1 */
        { 10000,  920,   0 }, /* 1 CR -> WS { 0, 1, 2 } */
        { 10000, 3702,  49 }, /* 2 CR -> WS { 0, 1, 2 } */
        { 10000, 3589, 246 }, /* 3 CR -> WS { 0, 1, 2 } */
    },
    { // level 2
        { 10000,  784,   0 },
        { 10000, 2740,  42 },
        { 10000, 2368, 123 },
    },
    { // level 3
        { 10000,  551,   0 },
        { 10000, 2461,  22 },
        { 10000, 2122,  14 },
    },
    { // level 4
        { 10000,  0,     0 },
        { 10000, 2179,   0 },
        { 10000, 1889,  10 },
    },
};

inline const std::vector<std::vector<std::vector<int>>> CR_REPLACEMENT_WS_COUNT_EXTRA_WILDS = {
    { /* level 1 */
        { 10000,  920,   0 }, /* 1 CR -> WS { 0, 1, 2 } */
        { 10000, 4312,  49 }, /* 2 CR -> WS { 0, 1, 2 } */
        { 10000, 4515,  343 }, /* 3 CR -> WS { 0, 1, 2 } */
    },
    { // level 2
        { 10000,  784,   0 },
        { 10000, 3116,  59 },
        { 10000, 2898, 179 },
    },
    { // level 3
        { 10000,  551,   0 },
        { 10000, 2773,  32},
        { 10000, 2582,  16 },
    },
    { // level 4
        { 10000,  0,     0 },
        { 10000, 2449,   0 },
        { 10000, 2283,  12 },
    },
};

inline const std::vector<std::vector<int>> CR_REPLACEMENT_CS_WEIGHTS_SET_A = {
    { /* level 1 -> 1x */
        65210,
        43470,
        13670,
        3540,
        1410,
        350,
        65,
        35,
        25,
        10,
        7,
        3,
        1,
    },
    { /* level 2 -> 2x */
        71710,
        43470,
        13670,
        3540,
        1110,
        280,
        58,
        28,
        18,
        9,
        4,
        2,
        0,
    },
    { /* level 3 -> 3x */
        79510,
        43470,
        13270,
        3140,
        1010,
        270,
        56,
        26,
        15,
        6,
        3,
        0,
        0,
    },
    { /* level 4 -> 10x */
        84510,
        43470,
        12670,
        2940,
        910,
        250,
        52,
        24,
        14,
        4,
        0,
        0,
        0,
    }
};

inline const std::vector<std::vector<int>> CR_REPLACEMENT_CS_WEIGHTS_SET_B = {
    { /* level 1 -> 1x */
        43210,
        43470,
        15670,
        4040,
        1510,
        450,
        85,
        35,
        25,
        10,
        7,
        5,
        1,
    },
    { /* level 2 -> 2x */
        50710,
        43470,
        15670,
        4040,
        1210,
        280,
        68,
        28,
        18,
        9,
        6,
        2,
        0,
    },
    { /* level 3 -> 3x */
        59510,
        43470,
        15670,
        4040,
        1110,
        270,
        56,
        26,
        16,
        7,
        3,
        0,
        0,
    },
    { /* level 4 -> 10x */
        67510,
        43470,
        15670,
        4040,
        1010,
        260,
        55,
        25,
        15,
        4,
        0,
        0,
        0,
    }
};

inline const std::vector<std::vector<int>> CR_REPLACEMENT_CS_WEIGHTS_SET_C = {
    { /* level 1 -> 1x */
        0,
        76680,
        25670,
        4040,
        1510,
        450,
        85,
        35,
        25,
        10,
        7,
        5,
        1
    },
    { /* level 2 -> 2x */
        0,
        84180,
        25670,
        4040,
        1210,
        280,
        68,
        28,
        18,
        9,
        6,
        2,
        0,
    },
    { /* level 4 -> 10x */
        0,
        92980,
        25670,
        4040,
        1110,
        270,
        56,
        26,
        16,
        7,
        3,
        0,
        0,
    },
    { /* level 3 -> 3x */
        0,
        100980,
        25670,
        4040,
        1010,
        260,
        55,
        25,
        15,
        4,
        0,
        0,
        0,
    }
};


// Scatter hook probs
constexpr inline const double SCATTER_HOOK_PROB_REGULAR     = 0.0425;
constexpr inline const double SCATTER_HOOK_PROB_BONUS_BOOST = 0.0995;
constexpr inline const double SCATTER_HOOK_PROB_SUPER_BOOST = 0.2;
