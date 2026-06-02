// GameData.h
#ifndef DATA_DEFS_H
#define DATA_DEFS_H

#include <cstdint>
#include <string>
#include <vector>
#include "constants.h"
#include "rng.h"
#include "reelset.h"
#include "bets.h"
#include "symbols.h"

enum class FsFeature {
    MORE_WILDS = 0,
    START_ON_LEVEL_2,
    REMOVE_LOWEST,
    MORE_COINS,
    EXTRA_2_SPINS,
    NONE,
};

struct PayTableStruct {
    Symbol name;
    int frequency;
    int win;
};

struct Coordinate {
    int x;  // X-coordinate
    int y;  // Y-coordinate
    Coordinate(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
};
using Coordinate_Vec = std::vector<Coordinate>;

struct SymCoords: Coordinate {
    Symbol symbol;
    SymCoords(int x, int y, Symbol sym):Coordinate::Coordinate(x, y), symbol(sym) {};
};
using SymCoords_Vec = std::vector<SymCoords>;

struct CoinSymbol {
    int     x        = -1;
    int     y        = -1;
    Symbol  symbol   = CS;
    int64_t valueX   = 0;
    int64_t refValue = 0;
    int64_t value    = 0;
};

struct WildCollection {
    int soFar          = 0;
    int triggeredLevel = 0;
    int activeLevel    = 0;
    int landed         = 0;
    int64_t coinMulti  = 0;
};

using CoinSym_Vec = std::vector<CoinSymbol>;

enum class ReelSetWeightSet {
    A,
    B,
    None,
};

struct LockedSymbol {
    Coordinate source;
    Coordinate destination;
    Symbol     symbol;

    LockedSymbol(Coordinate s, Coordinate d, Symbol sym): source(s), destination(d), symbol(sym) {};
};
using LockedSym_Vec = std::vector<LockedSymbol>;

struct BonusSymCoords: SymCoords {
    bool placed;
    BonusSymCoords(int x, int y, Symbol sym, bool placed):SymCoords::SymCoords(x, y, sym), placed(placed) {};
};

struct Winning {
    std::string type  = "normal";
    int64_t winAmount = 0;
    int64_t refWinAmount = 0;
    int payLine       = -1;
    int symCount      = 0;
    int multiplier    = 1;

    std::string dir;
    Symbol symbol;
    std::vector<Coordinate> coords;
};

using Winning_Vec = std::vector<Winning>;
using Grid = std::array<std::array<Symbol, GRID_HEIGHT>, REEL_COUNT>;
using GridV = std::vector<std::vector<Symbol>>;

struct Window {
    Grid grid;
    bool filled = false;
};

struct Spin {
    std::string reelSet;
    int index = 0;

    int64_t winAmount         = 0;
    int64_t refWinAmount      = 0;
    int64_t baseWinAmount     = 0;
    int64_t refBaseWinAmount  = 0;
    int64_t winsSoFar         = 0;
    int64_t refWinsSoFar      = 0;
    int64_t coinSymbolWins    = 0;
    int64_t refCoinSymbolWins = 0;

    int wsCollected           = 0;
    int wsCollectedSoFar      = 0;
    int newWsLanded           = 0;
    bool maxWinTriggered      = false;
    bool scatterHook          = false;
    int numSC                 = 0;
    int numWS                 = 0;
    int numCS                 = 0;
    int coinMultiplier        = 1;
    int level                 = -1;
    std::string type          = "";
    std::string coinValueSet  = "";
    int numCR                 = 0;
    bool wsForcedPlacement    = false;
    SymCoords_Vec newWsPlaced;
    SymCoords_Vec rSymReplacements;
    SymCoords_Vec overlayWsAndCrPlacements;
    SymCoords_Vec         bonusSyms;
    SymCoords_Vec         wildSyms;
    CoinSym_Vec           coinSyms;
    Grid                  stoppedGrid;
    Grid                  symbols;
    Grid                  scatterHookSymbols;
    LockedSym_Vec         lockedSymbols;
    Winning_Vec           winnings;
    std::array<int, REEL_COUNT>  stops;
    std::array<int, REEL_COUNT>  scatterHookStops{0, 0, 0, 0, 0};

#ifdef RAW_SYMMBOLS_INCL
    std::vector<std::vector<Symbol>> rawSymbols;
#endif
};

struct ReSpin: Spin {
    const char* spinType = "respin";
};
using Respin_Vec = std::vector<ReSpin>;

struct BaseSpin: Spin {
    const char* spinType = "basespin";
    std::vector<Spin> reSpins;
};

struct FreeSpin: Spin {
    const char* spinType     = "freespin";
    int64_t runningWinAmount = 0;
    Respin_Vec reSpins;
    // WildCollection wsCollected;

    int extraSpins = 0;
};

enum FS_STATUS {
    INIT,
    SUCCEEDED,
    FAILED,
    COLLECTED,
};

const std::map<std::string, FS_STATUS> FS_STATUS_MAP = {
    { "INIT", INIT },
    { "SUCCEEDED", SUCCEEDED },
    { "FAILED", FAILED },
    { "COLLECTED", COLLECTED }
};

struct FsStatus {
    int level;
    double draw = -1;
    FS_STATUS status;
};

struct Internal {
    std::vector<RandRecord> rHistory;
};

struct BuyExtraSpin {
    bool    extraSpin     = false;
    double expectedBet    = 0;
    int     featureMode   = -1;
    int     id            = -1;
    double costX          = -1.0;
};

struct PlayResponse {
    int64_t  betAmount;
    int64_t  refBetAmount;
    int64_t refBetBase;
    int  featureMode;
    bool extraSpin = false;
    bool persistent = true;
    bool subgameTriggered      = false;
    bool maxWinTriggered       = false;
    bool ended                 = false;
    bool empty                 = false;
    int64_t  winAmount    = 0;
    int64_t  refWinAmount = 0;
    int64_t currencyMultiplier = -1;

    std::vector<FsStatus> fsStatus;
    std::string           action;
    BaseSpin              baseSpin;
    std::vector<FreeSpin> freeSpins;
    Internal              internal;
    std::vector<FsFeature> activeFeatures;
    BuyExtraSpin           buyExtraSpin;
};

struct Options {
    bool          extraSpin = false;
    PlayResponse prevState;
};

struct PlayOptions {
    int         featureMode;
    int64_t     betAmount;
    int64_t     refBetAmount;
    int         buyFeature;
    int64_t     currencyMultiplier;
    std::string action;
    Options     options;
};

struct NextPlay {
    bool gamble;
};

#endif // DATA_DEFS_H
