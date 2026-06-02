#pragma once
#include <cstdint>
#include <map>
#include <string>
#include "../utils/error.h"
#include <vector>

// SC = Bonus Symbol
// WS = Wild and Collect symbol
// CS = Coin Symbol
// R  = Replacement symbol
// CR = Secondary Replacement symbol

enum Symbol {
    WS = 0,
    H1,
    H2,
    H3,
    H4,
    CS,
    L6,
    L7,
    L8,
    L9,
    L10,
    CR,
    SC,
    INVALID,
};

const char* const SymbolChar[] = {
    "WS",
    "H1",
    "H2",
    "H3",
    "H4",
    "CS",
    "L6",
    "L7",
    "L8",
    "L9",
    "L10",
    "CR",
    "SC",
    "INVALID",
};

const std::vector<Symbol> RegularPaySymbols = { H1, H2, H3, H4, L6, L7, L8, L9 };
const int REMOVE_LOWEST_COIN_2X_IDX = 0;
const int REMOVE_LOWEST_COIN_5X_IDX = 1;
const std::vector<int64_t> COIN_VALUES_X = {
    2, 5, 10, 15, 20,
    25, 50, 100, 200, 500,
    1666, 2500, 5000
};

const std::map<std::string, Symbol> SymMap = {
    { "WS",      WS },
    { "H1",      H1 },
    { "H2",      H2 },
    { "H3",      H3 },
    { "H4",      H4 },
    { "CS",      CS },
    { "L6",      L6 },
    { "L7",      L7 },
    { "L8",      L8 },
    { "L9",      L9 },
    { "L10",     L10 },
    { "CR",      CR },
    { "SC",      SC },
    { "INVALID", INVALID }
};

inline Symbol getSymbol (const std::string& symStr) {
    auto symbolIter = SymMap.find (symStr);
    if (symbolIter == SymMap.end())
        throw_error (symStr, " is not a valid symbol");

    return SymMap.at (symStr);
}

const enum Symbol SYM_ARR[] = { WS, H1, H2, H3, H4, CS, L6, L7, L8, L9, L10, CR, SC, INVALID, };
