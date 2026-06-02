#pragma once

#include <vector>
#include <iostream>
#include "./paywindow.h"
#include "./constants.h"

struct LineWinData {
    Symbol symbol;
    int    totalCount;
    int    WSCount;
};

using PayLine  = std::array<int, REEL_COUNT>;
using PayLines = std::array<PayLine, NUM_PAYLINES>;

const PayLines PAYLINES {{
    { 1, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0 },
    { 2, 2, 2, 2, 2 },
    { 0, 1, 2, 1, 0 },
    { 2, 1, 0, 1, 2 },
    { 0, 0, 1, 2, 2 },
    { 2, 2, 1, 0, 0 },
    { 1, 2, 2, 2, 1 },
    { 1, 0, 0, 0, 1 },
    { 2, 1, 1, 1, 0 },
}};
