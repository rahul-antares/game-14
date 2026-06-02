#include <cstdint>

#include "./paywindow.h"
#include "../utils/log.h"
#include "../utils/error.h"
#include "bets.h"
#include "constants.h"
#include "extra-spin.h"
#include "paytable.h"
#include "data-defs.h"
#include "paylines.h"
#include "reelset.h"
#include "symbols.h"

PayWindow::PayWindow(
    RngLib& rng,
    ReelSet& reelset
) :
    rng(rng),
    reelset(reelset)
{
    clear();
}

void PayWindow::selectReelSetByNameAndWeights (const int mode, const std::string& baseGameReelset) {
    enum ReelSetsEnums x;
    switch (mode) {
        case MODE_NORMAL   : x = REELSET_RG; break;
        case MODE_ENHANCED : x = REELSET_BB; break;
        case MODE_SB       : x = REELSET_SB; break;
        default: throw_error ("invalid mode in base-spin, mode = ", mode);
    }

    const std::vector<int> index1OnlyWeights = {
        1, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0,
    };

    reelset.select (x, true, /*forced=*/ false, index1OnlyWeights);
    const std::string index1ReelSet = getReelSetName ();

    std::pair<std::string, std::string> reelSetsToChoose;
    reelSetsToChoose.first = index1ReelSet;
    reelSetsToChoose.second = baseGameReelset;

    int weightsToChooseRS[] = {198, 3605};
    const int rsTochooseIdx = rng.getWeightedIndex(weightsToChooseRS, 2);

    std::string targetReelSet = (rsTochooseIdx == 1 ? reelSetsToChoose.second : reelSetsToChoose.first);
    reelset.selectReelSetByName (x, targetReelSet);
}

void PayWindow::selectReelset(bool baseSpin, bool moreCoins, int mode, bool forced, ReelSetWeightSet rsWeightSet) {
    enum ReelSetsEnums x;

    if (baseSpin) {
        switch (mode) {
            case MODE_NORMAL   : x = REELSET_RG; break;
            case MODE_ENHANCED : x = REELSET_BB; break;
            case MODE_SB       : x = REELSET_SB; break;
            default:
                throw_error ("invalid mode in base-spin, mode = ", mode);
        }

        auto weights = (rsWeightSet == ReelSetWeightSet::A ? Reel_Set_Weight_SetA: Reel_Set_Weight_SetB);
        reelset.select (x, baseSpin, forced, weights);
    } else {
        x = (moreCoins ? REELSET_FS_WINNER: REELSET_FS);
        reelset.select (x, false, forced);
    }
}

std::array<int, REEL_COUNT> PayWindow::spin (bool baseSpin, const int mode, bool reSpin, bool forced, bool moreCoins, int fsStopIdx) {
    std::array<int, REEL_COUNT> stops = reelset.spin (baseSpin, mode, reSpin, forced, moreCoins, fsStopIdx);
    Reels_V& reels = reelset.getSelected()->getReels();

    for (unsigned int i = 0; i < stops.size(); i++) {
        if (i >= REEL_COUNT)
            throw_error("too many stops returned in PayWindow::spin()");

        Reel* reel = reels.at(i);

        for (unsigned int j = 0; j < GRID_HEIGHT; j++)
            window[i][j] = reel->symbolAt(stops[i] + j);
    }

    printGrid("fresh spin");
    return stops;
}

void PayWindow::applyNudgingReels (std::vector<std::pair<int, int>>& nudgingReelsMeta, std::string nudgingReelSet, const int mode) {
    std::string currentSelectedReelSet = getReelSetName();
    enum ReelSetsEnums x;

    switch (mode) {
        case MODE_NORMAL   : x = REELSET_RG; break;
        case MODE_ENHANCED : x = REELSET_BB; break;
        case MODE_SB       : x = REELSET_SB; break;
        default: throw_error ("invalid mode in base-spin, mode = ", mode);
    }

    // select the nudging reelset
    reelset.selectReelSetByName(x, nudgingReelSet);
    Reels_V& reels = reelset.getSelected()->getReels();

    for (auto& [i, stop]: nudgingReelsMeta) {
        if (i >= REEL_COUNT)
            throw_error("too many stops returned in PayWindow::spin()");

        Reel* reel = reels.at(i);
        stop       = (stop - 1 + reel->size()) % reel->size(); // nudge down 1 step

        for (unsigned int j = 0; j < GRID_HEIGHT; j++)
            window[i][j] = reel->symbolAt(stop + j);
    }

    // reset the reelset selection
    reelset.selectReelSetByName(x, currentSelectedReelSet);
}

int PayWindow::symbolCount(Symbol s) {
    int symCount = 0;

    for(int i=0; i<REEL_COUNT; i++) {
        for(int j=0; j<GRID_HEIGHT; j++) {
            if (window[i][j] == s)
                symCount += 1;
        }
    }

    return symCount;
}

void PayWindow::drawCsSymbols (CoinSym_Vec& coinSymbols, const int64_t refBetBase) {
    coinSymbols.clear();

    if (CoinSymbolsWeight.size() != COIN_VALUES_X.size())
        throw_error ("invalid coin configuration, base-spSin");

    for (int i = 0; i < REEL_COUNT; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            if (window[i][j] == CS) {
                const auto idx    = rng.getWeightedIndex(CoinSymbolsWeight.data(), CoinSymbolsWeight.size());
                const auto valueX = COIN_VALUES_X[idx];

                coinSymbols.push_back ({
                    i, j, CS,
                    valueX,
                    valueX * refBetBase,
                    valueX * refBetBase,
                });
            }
        }
    }
}

void PayWindow::clear() {
    LOG("[paywindow] cleared\n");
}

Grid PayWindow::snapshot() {
    Grid grid;

    for (int i = 0; i < REEL_COUNT; i++)
        for (int j = 0; j < GRID_HEIGHT; j++)
            grid.at(i).at(j) = window[i][j];

    return grid;
}

// Optimized version
void PayWindow::snapshot(Grid& grid) {
    for (int i = 0; i < REEL_COUNT; i++)
        for (int j = 0; j < GRID_HEIGHT; j++)
            grid[i][j] = window[i][j];
}

// NOTE: implement this later
// void PayWindow::snapshot (GridV& grid) {
//     for (int i = 0; i < REEL_COUNT; i++) {
//         for (int j = 0; j < GRID_HEIGHT; j++) {
//             grid[i][j] = window[i][j];
//         }
//     }
// }

std::pair<int64_t, bool> PayWindow::getWinnings (
    Winning_Vec& winnings,
    const int refBetBase,
    int64_t refWinsSoFar
) {
    LineWinData refWinData;
    const PayLines& lines = PayTableCtrl::lines();
    int64_t refWinAmount  = 0;
    int64_t maxWinAmount  = MAX_WIN_CAP * refBetBase;
    bool maxWinTriggered  = false;

    LOG ("Win Amount at the start of payline-win evaluation = %ld\n", refWinAmount);
    LOG("%sEvaluating Winnings using #Paylines = %zu%s\n", ANSI_COLOR_GREEN, lines.size(), ANSI_COLOR_RESET);

    for (unsigned int i = 0; i < lines.size(); i++) {
        const PayLine& line = lines[i];
        LOG("%sEvaluating #payline %d%s\n", ANSI_COLOR_YELLOW, i+1, ANSI_COLOR_RESET);

        PayTableCtrl::evaluateLine (window, line, i, refWinData);
        auto [ symbol, totalCount, WSCount ] = refWinData;
        if (totalCount < 2)
            continue;

        // Perhaps this test is not needed
        if (symbol == SC)
            return std::make_pair(0, maxWinTriggered);

        int multi             = PayTable[symbol][totalCount - 2];
        int64_t currWinAmount = (refBetBase * multi) / CREDITS_PER_BET;
        maxWinTriggered       = (refWinAmount + refWinsSoFar + currWinAmount) >= maxWinAmount;
        currWinAmount         = maxWinTriggered ? (maxWinAmount - refWinsSoFar - refWinAmount) : currWinAmount;

        LOG ("Win Amount = %ld, wAmount = %ld, multi = %d, credit multiplier / refBetBase = %d\n", refWinAmount, currWinAmount, multi, refBetBase / CREDITS_PER_BET);
        if (currWinAmount) {
            refWinAmount     += currWinAmount;

            if (maxWinTriggered) {
                LOG("Max Win Triggered in Payline Win Evaluation\n");
                break;
            }

            Winning win;
            win.payLine      = i;
            win.dir          = "ltr";
            win.symbol       = symbol;
            win.symCount     = totalCount;
            win.multiplier   = multi;
            win.refWinAmount = currWinAmount;
            win.coords       = PayTableCtrl::getCoords(line, totalCount);

            winnings.push_back(win);
            LOG("\n");
        }
    }

#ifdef DEBUG_LOG_ENABLED
    // printHighlightWinningSyms(winnings);
#endif

    LOG ("Win Amount = %ld\n", refWinAmount);
    return std::make_pair(refWinAmount, maxWinTriggered);
}

void PayWindow::printGrid(const char* message) {
#ifdef DEBUG_LOG_ENABLED
    LOG("[paywindow] state @ %s%s%s\n", ANSI_COLOR_MAGENTA, message, ANSI_COLOR_RESET);

    LOG("     " ANSI_COLOR_CYAN);
    for (int i = 0; i < REEL_COUNT; i++) {
        LOG("%-7d", i);
    }
    LOG("\n" ANSI_COLOR_RESET);

    for (int i = 0; i < GRID_HEIGHT; i++) {
        LOG(ANSI_COLOR_CYAN " %2d " ANSI_COLOR_RESET, i);

        for (int j = 0; j < REEL_COUNT; j++) {
            Symbol symbol = window[j][i];
            const char* color;

            switch (symbol) {
                case WS: color = ANSI_COLOR_GREEN; break;
                case CR: color = ANSI_COLOR_MAGENTA; break;
                case SC: color = ANSI_BG_COLOR_BLUE; break;
                case CS: color = ANSI_BG_COLOR_CYAN; break;
                default: color = ANSI_COLOR_RESET; break;
            }

            if (symbol == CS)
                color = ANSI_BG_COLOR_MAGENTA;

            LOG(" %s%-6s%s", color, SymbolChar[symbol], ANSI_COLOR_RESET);
        }

        LOG("\n");
    }

    // Print reel stop positions
    LOG("     " ANSI_COLOR_YELLOW);
    for (int i : reelset.getStops()) {
        LOG("%-7d", i);
    }
    LOG("\n" ANSI_COLOR_RESET);
#endif
}

void PayWindow::printWinSymsMap (std::map<Symbol, WinSym>& symMap) {
#ifdef DEBUG_LOG_ENABLED
    for (auto iter = symMap.begin(); iter != symMap.end(); iter++) {
        Symbol sym  = iter->first;
        WinSym data = iter->second;
        LOG("Win Symbol \"%s\"(count = %d)\n", SymbolChar[sym], data.count);

        for (Coordinate c : data.coordinates)
        LOG("  - (%d, %d)\n", c.x, c.y);
    }
#endif
}

// -> [(stop, reel_idx), ...]
std::vector<std::pair<int, int>> PayWindow::scatterHookPositions (const std::vector<int>& nonScSymReels, const std::array<int, REEL_COUNT>& spinStops) {
    std::vector<std::pair<int, int>> positions;
    Reels_V& reels = reelset.getSelected()->getReels();

    for (auto reelIdx: nonScSymReels) {
        if (reelIdx >= REEL_COUNT)
            throw_error("too many stops returned in PayWindow::scatterHoookPositions()");

        Reel* reel            = reels.at(reelIdx);
        bool found            = false;
        int stop              = spinStops [reelIdx];
        const int reelSize    = reel->size();
        const int stopOneDown = (stop + 1) % reelSize;
        const int stopTwoDown = (stop + 2) % reelSize;
        const int gridHeightDiff = GRID_HEIGHT-1;

        // check 1 pos below
        if (reel->symbolAt ((stopOneDown + gridHeightDiff) % reelSize) == SC) {
            found = true;
            stop  = stopOneDown;
        }

        if (!found && reel->symbolAt ((stopTwoDown + gridHeightDiff) % reelSize) == SC) {
            found = true;
            stop  = stopTwoDown;
        }

        if (found)
            positions.push_back ({ stop % reelSize, reelIdx });
    }

     return positions;
}

bool PayWindow::scatterHook (Spin& _spin, const int mode) {
    if (_spin.numSC != 2)
        return false;

    double shfProb = -0.01;
    switch (mode) {
        case MODE_NORMAL  : shfProb = SCATTER_HOOK_PROB_REGULAR; break;
        case MODE_ENHANCED: shfProb = SCATTER_HOOK_PROB_BONUS_BOOST; break;
        case MODE_SB      : shfProb = SCATTER_HOOK_PROB_SUPER_BOOST; break;
        default: throw_error ("invalid mode = ", mode);
    }

    const bool shouldShfTrig = (rng.getDouble() < shfProb);

    std::vector<int> nonScSymReels;
    for (int i = 0; i < REEL_COUNT; i++) {
        bool hasScatter = false;

        for (int j = 0; j < GRID_HEIGHT; j++) {
            if (window[i][j] == SC) {
                hasScatter = true;
                break;
            }
        }

        if (!hasScatter)
            nonScSymReels.push_back (i);
    }

    if (nonScSymReels.size() != 3)
        throw_error ("Should have 3 reels without SC symbols, found ", nonScSymReels.size());

    // looking for SC symbol 1 or 2 positions below, (stop, reel_idx)
    std::vector<std::pair<int, int>> scatterHookStops = scatterHookPositions (nonScSymReels, _spin.stops);

    bool shfTriggered = false;

    if (scatterHookStops.size() > 0 && shouldShfTrig) {
        std::pair<int, int> scatterHookStop = scatterHookStops [rng.getScaled (scatterHookStops.size())];
        _spin.scatterHookStops = _spin.stops;
        _spin.scatterHookStops [scatterHookStop.second] = scatterHookStop.first;
        _spin.scatterHook = true;

        // generate the grid based on the scatter hook stops

        Reels_V& reels = reelset.getSelected()->getReels();
        for (unsigned int i = 0; i < _spin.scatterHookStops.size(); i++) {
            if (i >= REEL_COUNT)
                throw_error("too many stops returned in PayWindow::spin()");

            Reel* reel = reels.at(i);

            for (unsigned int j = 0; j < GRID_HEIGHT; j++)
                window[i][j] = reel->symbolAt(_spin.scatterHookStops[i] + j);
        }

        _spin.numSC  = symbolCount(SC);
        shfTriggered = true;
    }

    return shfTriggered;
}

bool PayWindow::shouldReSpin () {
    bool scSymsOnBottomRow = false;

    for (int i = 0; i < REEL_COUNT; i++) {
        if (window[i][GRID_HEIGHT-1] == SC) {
            scSymsOnBottomRow = true;
            break;
        }
    }

    return !scSymsOnBottomRow;
}


int PayWindow::getNumWSToPlace (const bool extraWilds, const int wsColLevel, const bool firstExtraSpin, const bool gtdWildSpecial) {
    const int numCR = symbolCount (CR);
    if (numCR <= 0)
        return 0;

   // numCR:idx -> 1:0, 2:1, >=3:2
    const int numCRIdx = (numCR >= 3 ? 2: numCR-1);

    if (wsColLevel < 1 || wsColLevel > 4 || numCRIdx < 0 || numCRIdx > 2) // just to be safe for num-cr-idx
        throw_error ("invalid ws-col level bounds, should be [1, 4], curr-ws-level = ", wsColLevel, ", numCrIdx = ", numCRIdx);

    const auto& wsCountCRReplaceWeights = [&]() -> const auto& {
        if (firstExtraSpin) {
            return extraWilds ? EXTRA_SPIN_CR_REPLACEMENT_WS_COUNT_EXTRA_WILDS 
                : EXTRA_SPIN_CR_REPLACEMENT_WS_COUNT_NO_EXTRA_WILDS;
        } else {
            return extraWilds ? CR_REPLACEMENT_WS_COUNT_EXTRA_WILDS 
                : CR_REPLACEMENT_WS_COUNT_NO_EXTRA_WILDS;
        }
    }();

    const auto& levelWeightsCRReplace   = wsCountCRReplaceWeights [wsColLevel-1];
    const auto& numWSWeights            = levelWeightsCRReplace[numCRIdx];
    // const int numWSToPlace              = rng.getWeightedIndex (numWSWeights.data(), numWSWeights.size());
    const int weightsMod[] = {10000, 1500};
    const int numWSToPlace = rng.getWeightedIndex (weightsMod, 2);

    if (numWSToPlace > 2)
        throw_error ("too many ws symbols to place, numWSToPlace = ", numWSToPlace);

    // if (numWSToPlace == 0 && gtdWildSpecial)
    //     return 1;

    return numWSToPlace;
}

int PayWindow::getCoinSymbolForCRRep (const int& wsColLevel, const CoinSet& coinSet, const bool removeLowest, const bool extraSpin) {
    const int wsOnGrid = symbolCount (WS);
    int coinSymIdx     = -1;
    std::vector<int> weights;

    if (wsOnGrid == 0) {
        weights = CoinSymbolsWeight;
    } else {
        if (coinSet == CoinSet::None)
            throw_error ("coin set not selected");

        const bool coinSetC = (coinSet != CoinSet::A && removeLowest && extraSpin);
        weights = (
                coinSetC ? CR_REPLACEMENT_CS_WEIGHTS_SET_C :
                coinSet == CoinSet::A ? CR_REPLACEMENT_CS_WEIGHTS_SET_A :
                CR_REPLACEMENT_CS_WEIGHTS_SET_B
            )[wsColLevel-1];
    }

    if (removeLowest) {
        // ---- Approach 1: Remove 2x from draw pool
        // std::vector<int> weightsExclude2x = weights;
        // weightsExclude2x.erase (weightsExclude2x.begin() + 0);

        // // kind of redundant check, but still for peace of mind
        // if (weightsExclude2x.empty() || weightsExclude2x.size() >= weights.size())
        //     throw_error ("coin weights list empty");

        // coinSymIdx = rng.getWeightedIndex (weightsExclude2x.data(), weightsExclude2x.size());

        // if (weightsExclude2x.size() < weights.size())
        //     coinSymIdx += (weights.size() - weightsExclude2x.size());

        // ---- Approach 2: Replace 2x after draw with 5x
        coinSymIdx = rng.getWeightedIndex (weights.data(), weights.size());
        if (coinSymIdx == REMOVE_LOWEST_COIN_2X_IDX)
            coinSymIdx = REMOVE_LOWEST_COIN_5X_IDX;

    } else {
        coinSymIdx = rng.getWeightedIndex (weights.data(), weights.size());
    }

    if (coinSymIdx < 0 || (coinSymIdx == REMOVE_LOWEST_COIN_2X_IDX && removeLowest))
        throw_error ("not a valid coin, coinSymIdx = ", coinSymIdx);

    return coinSymIdx;
}

void PayWindow::placeCoinSymbolsForCR (
    CoinSym_Vec& coinSymbols,
    const int64_t& refBetBase,
    const WildCollection& wsCol,
    const CoinSet& coinSet,
    const bool removeLowest,
    const bool extraSpin
) {
    coinSymbols.clear();

    for (int i = 0; i < REEL_COUNT; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            if (window[i][j] == CR) {
                window[i][j]      = CS;
                const auto idx    = getCoinSymbolForCRRep (wsCol.activeLevel, coinSet, removeLowest, extraSpin);
                const auto valueX = COIN_VALUES_X [idx];

                if (valueX == 2 && removeLowest)
                    throw_error ("remove_lowest feature active with 2x coin");

                coinSymbols.push_back ({
                    i, j, window[i][j],
                    valueX,
                    valueX * refBetBase,
                    valueX * refBetBase,
                });
            }
        }
    }

    printGrid("after coin symbol placement");
}

void PayWindow::replaceCR (
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
) {
    SymCoords_Vec crSymbols;
    std::vector<int> crSymIdxs;

    for (int i = 0; i < REEL_COUNT; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            if (window[i][j] == CR)
                crSymbols.push_back({i, j, window[i][j]});
        }
    }

    for (int i = 0; i < crSymbols.size(); i++)
        crSymIdxs.push_back (i);

    const int numWSToPlace = getNumWSToPlace (extraWilds, wsCol.triggeredLevel, firstExtraSpin, gtdWildSpecial);
    int nWSPlace           = numWSToPlace;

    while (nWSPlace > 0) {
        const int randIdx    = rng.getScaled(crSymIdxs.size());
        const int crSymIdx   = crSymIdxs [randIdx];
        const auto& crSymbol = crSymbols [crSymIdx];

        const int x          = crSymbol.x;
        const int y          = crSymbol.y;
        window[x][y]         = WS;

        wildSyms.push_back ({ x, y, window[x][y]});
        crSymIdxs.erase (crSymIdxs.begin() + randIdx);

        nWSPlace--;
    }

    placeCoinSymbolsForCR (coinSymbols, refBetBase, wsCol, coinSet, removeLowest, extraSpin);
}

void PayWindow::printHighlightWinningSyms (std::map<Symbol, WinSym> symMap) {
#ifdef DEBUG_LOG_ENABLED

    printWinSymsMap(symMap);
    std::map<Symbol, WinSym> filtered;

    // Filter out the entries with no winnings
    for (auto iter = symMap.begin(); iter != symMap.end(); iter++) {
        Symbol sym  = iter->first;
        WinSym data = iter->second;

        /*if (data.count >= PayoutThresholds[0])*/
        /*    filtered[sym] = data;*/
    }

    LOG("[paywindow] " ANSI_COLOR_GREEN "highlighting winning symbols (%lu)\n", filtered.size());
    LOG("   " ANSI_COLOR_CYAN);
    for (int i = 0; i < REEL_COUNT; i++) {
        LOG("%5d", i);
    }
    LOG("\n" ANSI_COLOR_RESET);

    for (int i = 0; i < GRID_HEIGHT; i++) {
        LOG(ANSI_COLOR_CYAN "  %d " ANSI_COLOR_RESET, i);

        for (int j = 0; j < REEL_COUNT; j++) {
            Symbol symbol = window[j][i];
            const char* color = ANSI_COLOR_RESET;

            if (filtered.count(symbol) != 0) {
                color = ANSI_COLOR_GREEN ANSI_SLOW_BLINK;
            }
            else {
                switch (symbol) {
                    case WS: color = ANSI_BG_COLOR_GREEN; break;
                    case CR: color = ANSI_BG_COLOR_MAGENTA; break;
                    case SC: color = ANSI_BG_COLOR_BLUE; break;
                    case CS: color = ANSI_BG_COLOR_CYAN; break;
                    default: color = ANSI_COLOR_RESET; break;
                }
            }

            LOG(". %s%2s%s ",
                color,
                SymbolChar[symbol],
                ANSI_COLOR_RESET
                );
        }
        LOG(".\n");
    }

    // Print reel stop positions
    LOG("   " ANSI_COLOR_YELLOW);
    for (int i : reelset.getStops()) {
        LOG("%5d", i);
    }
    LOG(ANSI_COLOR_RESET "\n");

#endif
}

