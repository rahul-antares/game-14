#pragma once
#include <vector>
#include "constants.h"
#include "paylines.h"
#include "symbols.h"

struct PayTbl {
    Symbol symbol;
    int    pay;
    int    count;
};

using PayTblItem = std::array<int, 4>;
using PayTableArray = std::array<PayTblItem, 11>;

const std::vector<PayTbl> PayTableConf {
    { H1, 2000, 5 },
    { H1, 200, 4 },
    { H1, 50, 3 },
    { H1, 5, 2 },
    { H2, 1000, 5 },
    { H2, 150, 4 },
    { H2, 30, 3 },
    { H2, 0, 2 },
    { H3, 500,  5 },
    { H3, 100, 4 },
    { H3, 20, 3 },
    { H3, 0, 2 },
    { H4, 500,  5 },
    { H4, 100, 4 },
    { H4, 20, 3 },
    { H4, 0, 2 },
    { CS, 200,  5 },
    { CS, 50, 4 },
    { CS, 10, 3 },
    { CS, 0, 2 },
    { L6, 100,  5 },
    { L6, 25,  4 },
    { L6, 2,  3 },
    { L6, 0, 2 },
    { L7, 100,  5 },
    { L7, 25,  4 },
    { L7, 2,  3 },
    { L7, 0, 2 },
    { L8, 50,  5 },
    { L8, 10,  4 },
    { L8, 2,  3 },
    { L8, 0, 2 },
    { L9, 50,  5 },
    { L9, 10,  4 },
    { L9, 2,  3 },
    { L9, 0, 2 },
    { L10, 50, 5 },
    { L10, 10, 4 },
    { L10, 2, 3 },
    { L10, 0,  2},
};

const PayTableArray PayTable {{
    /* sym count =  2  3   4    5 */
    /* WS  */     { 0,  0,   0,    0,},
    /* H1  */     { 5, 50, 200, 2000,},
    /* H2  */     { 0, 30, 150, 1000,},
    /* H3  */     { 0, 20, 100,  500,},
    /* H4  */     { 0, 20, 100,  500,},
    /* CS  */     { 0, 10, 50, 200,},
    /* L6  */     { 0,  2,  25,  100,},
    /* L7  */     { 0,  2,  25,  100,},
    /* L8  */     { 0,  2,  10,   50,},
    /* L9  */     { 0,  2,  10,   50,},
    /* L10 */     { 0,  2,  10,   50,},
}};

const std::vector<std::string> StateNames = { "INIT", "WILD", "WILD_2", "FIRST_SYM", "LATCHED", "NO_WIN", "WIN", "INVALID_STATE" };

class PayTableCtrl {
    public:
        enum State { INIT, WILD, FIRST_SYM, LATCHED, NO_WIN, WIN, INVALID_STATE };
        enum Token { SYM_WS, SYM_OTHER, SYM_N1, SYM_NX, SYM_END };

        static inline const PayLines& lines() { return PAYLINES; }
        static void evaluateLine(const Symbol (&window)[REEL_COUNT][GRID_HEIGHT], const PayLine& line, const int id, LineWinData& winData);
        static Coordinate_Vec getCoords(const PayLine& line, int symCount);

    private:
        static State nextState(Token token, State state);
};
