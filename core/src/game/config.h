#include "bets.h"
#include "reelset.h"
#include <vector>
#include "paytable.h"
#include "paylines.h"
#include "extra-spin.h"
#include "round.h"

// TODO: add coin values to the config
struct GameConfig {
    unsigned int                     creditsPerBet;
    unsigned int                     maxWinCap;
    const std::vector<PayTbl>&       payTable        = PayTableConf;
    const std::vector<BetConfig>&    betConfig       = ALL_BETS_CONFIG;
    const std::vector<EsBetConfig>&  esbetConfig     = ES_BET_CONFIGS;
    const SetOfReelSets*             rgsets;
    const SetOfReelSets*             bbsets;
    const SetOfReelSets*             sbsets;
    const SetOfReelSets*             fssets;
    const char*                      gameCore        = GAME_CORE;
    const char*                      gameName        = GAME_NAME;
    const char*                      gameVersion     = GAME_VERSION;
    const char*                      gameStudio      = GAME_STUDIO;
    const char*                      variant         = VARIANT;
    const char*                      parsheetVersion = PARSHEET_VERSION;
    const double                     probFreeSpinsBB = IncProbFreespinsBB;
    const double                     probFreeSpinsSB = IncProbFreeSpinsSB;
    const PayLines&                  paylines        = PAYLINES;
    const std::array<double, MAX_GAMBLE_LEVEL> ladderWeights = LADDER_WEIGHTS;
};

