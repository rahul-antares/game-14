#include "paytable.h"
#include "../utils/log.h"
#include "constants.h"
#include "data-defs.h"
#include "symbols.h"

void PayTableCtrl::evaluateLine(const Symbol (&window)[REEL_COUNT][GRID_HEIGHT], const PayLine &line, const int id, LineWinData &winData) {
    Symbol latchedSym = INVALID;
    Token token;
    State state = INIT;
    int totalCount = 0, WSCount = 0;

    for(int x = 0; x < REEL_COUNT; x++) {
        int y = line[x];
        Symbol sym = window[x][y];
        // LOG("Evaluating the Co-ordinates (%d, %d) \n", x, y);
        // LOG("Symbol for win eval is %s\n", SymbolChar[sym]);

        if(sym == SC) token = SYM_OTHER;
        else if(sym == WS) token = SYM_WS;
        else if (sym == latchedSym) token = SYM_N1;
        else token = SYM_NX;

        state = nextState(token, state);

        if (sym == WS) WSCount++;
        if ((state == FIRST_SYM || state == LATCHED) && (sym != WS))
            latchedSym = sym;

        /*LOG("[Line:evaluate] sym \"%s\" at (%d, %d) (next state = \"%s\")(latched = \"%s\")\n",*/
        /*    SymbolChar[sym],*/
        /*    y, x,*/
        /*    StateNames[state].c_str(),*/
        /*    SymbolChar[latchedSym]*/
        /*    );*/
        if (state == WIN || state == NO_WIN)
            break;

        totalCount++;
    }

    state = nextState(SYM_END, state);

    if (state == WIN)
        LOG("[Line:evaluate] %s: %slatched \"%s\"(count=%d) on line %d%s, WS = %d\n",
            StateNames[state].c_str(),
            ANSI_COLOR_GREEN,
            SymbolChar[latchedSym], totalCount,
            id,
            ANSI_COLOR_RESET,
            WSCount
            );

    switch (state) {
        case WIN:
            winData.symbol     = latchedSym;
            winData.totalCount = totalCount;
            winData.WSCount    = WSCount;
            break;
        case NO_WIN:
            winData.symbol     = INVALID;
            winData.totalCount = 0;
            winData.WSCount    = 0;
            break;
        default:
            LOG("[Line:evaluate] %sERROR: line %d: %s: latched = \"%s\"(%d), WS = %d%s\n",
                ANSI_BG_COLOR_RED,
                id, StateNames[state].c_str(), SymbolChar[latchedSym], totalCount, WSCount,
                ANSI_COLOR_RESET
                );
            throw std::logic_error("invalid end state for line evaluation");
    }

    return;
}

PayTableCtrl::State PayTableCtrl::nextState(Token token, State state) {
    State next = INVALID_STATE;

    switch (state) {
        case INIT:
            if      (token == SYM_WS)     next = WILD;
            else if (token == SYM_N1)     next = FIRST_SYM;
            else if (token == SYM_NX)     next = FIRST_SYM;
            else if (token == SYM_OTHER)  next = NO_WIN;
            else if (token == SYM_END)    next = NO_WIN;
            break;

        case WILD:
            if (token == SYM_WS)  next = WILD;
            else if (token == SYM_N1)     next = LATCHED;
            else if (token == SYM_NX)     next = LATCHED;
            else if (token == SYM_OTHER)  next = NO_WIN;
            else if (token == SYM_END)    next = NO_WIN;
            break;

        case FIRST_SYM:
            if (token == SYM_WS)  next = LATCHED;
            else if (token == SYM_N1)     next = LATCHED;
            else if (token == SYM_NX)     next = NO_WIN;
            else if (token == SYM_OTHER)  next = NO_WIN;
            else if (token == SYM_END)    next = NO_WIN;
            break;

        case LATCHED:
            if (token == SYM_WS)  next = LATCHED;
            else if (token == SYM_N1)     next = LATCHED;
            else if (token == SYM_NX)     next = WIN;
            else if (token == SYM_OTHER)  next = WIN;
            else if (token == SYM_END)    next = WIN;
            break;

        case NO_WIN:
            next = NO_WIN;
            break;

        case WIN:
            next = WIN;
            break;

        case INVALID_STATE:
            next = INVALID_STATE;
            break;
    }

    return next;
}

Coordinate_Vec PayTableCtrl::getCoords(const PayLine& line, int symCount) {
    Coordinate_Vec coords;

    for (int x = 0; x < symCount; x++) {
        int y = line[x];

        coords.push_back(Coordinate{x, y});
    }

    return coords;
}
