#pragma once

#include <bitset>
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <iostream>
#include "../utils/error.h"
#include "bets.h"
#include "constants.h"

inline constexpr uint8_t MASK_MORE_WILDS    = 1 << 3;  // 0b1000
inline constexpr uint8_t MASK_REMOVE_LOWEST = 1 << 2;  // 0b0100
inline constexpr uint8_t MASK_MORE_COINS    = 1 << 1;  // 0b0010
inline constexpr uint8_t MASK_EXTRA_2_SPINS = 1 << 0;  // 0b0001

inline constexpr uint8_t ESI_TOTAL_SIZE     = 16;
inline constexpr uint8_t MAX_WILD_COL_SIZE  = 13;
inline constexpr int MODE_EXTRA_SPIN_END = MODE_EXTRA_SPIN_START + (ESI_TOTAL_SIZE * MAX_WILD_COL_SIZE);

struct ExtraSpinInfo {
    double costX        = -1.0;
    double expectedBet  = -1.0;
    int id              = -1;
    int featureMode     = MODE_EXTRA_SPIN_START;
    double coinSetAProb = -1.0;
    bool extraWilds     = false;
    bool removeLowest   = false;
    bool moreCoins      = false;
    bool extra2Spins    = false;
};

constexpr inline std::array<std::array<double, MAX_WILD_COL_SIZE>, ESI_TOTAL_SIZE> EXTRA_SPIN_COSTS = {{
    { 5.6, 5.6, 6.5, 26, 9.7, 9.7, 10.7, 34.8, 13.5, 13.5, 13.9, 73.3, 42.4 },
    { 6.5, 6.5, 8.1, 36.4, 11.4, 11.4, 13, 47.1, 15.6, 15.6, 16.3, 94.6, 49.2 },
    { 7.2, 7.2, 8.5, 34.4, 13, 13, 14.3, 47.6, 18.6, 18.6, 19.2, 103.3, 60.2 },
    { 7.4, 7.4, 9, 36.9, 12.9, 12.9, 14.4, 48.3, 17.8, 17.8, 18.4, 100, 55.9 },
    { 5.6, 5.6, 6.9, 33.2, 9.7, 9.7, 11, 43.1, 13.5, 13.5, 14, 85, 42.4 },
    { 8.4, 8.4, 10.5, 48.5, 15.3, 15.3, 17.5, 64.6, 21.6, 21.6, 22.5, 133.5, 69.9 },
    { 8.7, 8.7, 11.3, 53.2, 15.2, 15.2, 17.7, 66.7, 20.7, 20.7, 21.5, 130.6, 65.1 },
    { 6.5, 6.5, 8.7, 49.1, 11.4, 11.4, 13.6, 60.3, 15.6, 15.6, 16.4, 110.2, 49.2 },
    { 9.6, 9.6, 11.7, 49, 17.3, 17.3, 19.3, 66, 24.5, 24.5, 25.4, 140.9, 79.4 },
    { 7.2, 7.2, 8.9, 44.3, 13, 13, 14.7, 59.2, 18.6, 18.6, 19.3, 120, 60.2 },
    { 7.4, 7.4, 9.6, 48.1, 12.9, 12.9, 14.9, 60.5, 17.8, 17.8, 18.5, 116.3, 55.9 },
    { 11.3, 11.3, 14.7, 71, 20.3, 20.3, 23.8, 91.6, 28.6, 28.6, 29.7, 184.2, 92.4 },
    { 8.4, 8.4, 11.4, 66, 15.3, 15.3, 18.3, 83.2, 21.6, 21.6, 22.6, 155.5, 69.9 },
    { 8.7, 8.7, 12.5, 73.5, 15.2, 15.2, 18.7, 86.7, 20.7, 20.7, 21.7, 152.4, 65.1 },
    { 9.6, 9.6, 12.5, 64.1, 17.3, 17.3, 20, 83, 24.5, 24.5, 25.5, 164, 79.4 },
    { 11.3, 11.3, 16.3, 99, 20.3, 20.3, 25.2, 119.4, 28.6, 28.6, 29.9, 215, 92.4 },
}};

constexpr inline std::array<std::array<int, MAX_WILD_COL_SIZE>, ESI_TOTAL_SIZE> EXTRA_SPIN_IDS = {{
    { 1, 17, 33, 49, 65, 81, 97, 113, 129, 145, 161, 177, 193 },
    { 2, 18, 34, 50, 66, 82, 98, 114, 130, 146, 162, 178, 194 },
    { 3, 19, 35, 51, 67, 83, 99, 115, 131, 147, 163, 179, 195 },
    { 4, 20, 36, 52, 68, 84, 100, 116, 132, 148, 164, 180, 196 },
    { 5, 21, 37, 53, 69, 85, 101, 117, 133, 149, 165, 181, 197 },
    { 6, 22, 38, 54, 70, 86, 102, 118, 134, 150, 166, 182, 198 },
    { 7, 23, 39, 55, 71, 87, 103, 119, 135, 151, 167, 183, 199 },
    { 8, 24, 40, 56, 72, 88, 104, 120, 136, 152, 168, 184, 200 },
    { 9, 25, 41, 57, 73, 89, 105, 121, 137, 153, 169, 185, 201 },
    { 10, 26, 42, 58, 74, 90, 106, 122, 138, 154, 170, 186, 202 },
    { 11, 27, 43, 59, 75, 91, 107, 123, 139, 155, 171, 187, 203 },
    { 12, 28, 44, 60, 76, 92, 108, 124, 140, 156, 172, 188, 204 },
    { 13, 29, 45, 61, 77, 93, 109, 125, 141, 157, 173, 189, 205 },
    { 14, 30, 46, 62, 78, 94, 110, 126, 142, 158, 174, 190, 206 },
    { 15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207 },
    { 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208 },
}};

#if defined (VARIANT_96)
inline constexpr std::array<std::array<double, MAX_WILD_COL_SIZE>, ESI_TOTAL_SIZE> EXTRA_SPIN_COIN_SET_A_PROBS = {{
    { 0.2253, 0.2253, 0.305, 0.2501, 0.2927, 0.2927, 0.2325, 0.2524, 0.2272, 0.2272, 0.2518, 0.2332, 0.2385, },
    { 0.2854, 0.2854, 0.241, 0.25, 0.2661, 0.2661, 0.2531, 0.243, 0.2624, 0.2624, 0.2205, 0.2312, 0.2387, },
    { 0.2148, 0.2148, 0.2098, 0.2454, 0.2482, 0.2482, 0.2306, 0.2235, 0.2194, 0.2194, 0.2313, 0.2119, 0.2147, },
    { 0.3233, 0.3233, 0.2786, 0.2668, 0.2865, 0.2865, 0.2507, 0.2448, 0.2325, 0.2325, 0.2288, 0.236, 0.2329, },
    { 0.2253, 0.2253, 0.2378, 0.2606, 0.2927, 0.2927, 0.2344, 0.2502, 0.2272, 0.2272, 0.2384, 0.2364, 0.2385, },
    { 0.252, 0.252, 0.2394, 0.2321, 0.213, 0.213, 0.2087, 0.2221, 0.2179, 0.2179, 0.2101, 0.2068, 0.2106    , },
    { 0.3057, 0.3057, 0.2846, 0.2536, 0.2678, 0.2678, 0.2703, 0.2473, 0.2403, 0.2403, 0.2469, 0.2299, 0.2303, },
    { 0.2854, 0.2854, 0.2909, 0.2599, 0.2661, 0.2661, 0.2428, 0.2476, 0.2624, 0.2624, 0.2279, 0.2296, 0.2387, },
    { 0.2471, 0.2471, 0.2307, 0.2273, 0.2319, 0.2319, 0.2243, 0.218, 0.2311, 0.2311, 0.2071, 0.2111, 0.2031, },
    { 0.2148, 0.2148, 0.2666, 0.2344, 0.2482, 0.2482, 0.2429, 0.2173, 0.2194, 0.2194, 0.2364, 0.2053, 0.2147, },
    { 0.3233, 0.3233, 0.2624, 0.2489, 0.2865, 0.2865, 0.2468, 0.2412, 0.2325, 0.2325, 0.2345, 0.2316, 0.2329, },
    { 0.2301, 0.2301, 0.2663, 0.227, 0.2557, 0.2557, 0.2386, 0.2139, 0.2108, 0.2108, 0.2281, 0.2071, 0.2107, },
    { 0.252, 0.252, 0.2428, 0.2337, 0.213, 0.213, 0.2153, 0.2115, 0.2179, 0.2179, 0.2341, 0.2122, 0.2106    , },
    { 0.3057, 0.3057, 0.2622, 0.2565, 0.2678, 0.2678, 0.2538, 0.2374, 0.2403, 0.2403, 0.2286, 0.2304, 0.2303, },
    { 0.2471, 0.2471, 0.222, 0.2323, 0.2319, 0.2319, 0.2185, 0.214, 0.2311, 0.2311, 0.2292, 0.2058, 0.2031, },
    { 0.2301, 0.2301, 0.2665, 0.2223, 0.2557, 0.2557, 0.2215, 0.2188, 0.2108, 0.2108, 0.2333, 0.2103, 0.2107, },
}};
#elif defined (VARIANT_94)
constexpr inline std::array<std::array<double, MAX_WILD_COL_SIZE>, ESI_TOTAL_SIZE> EXTRA_SPIN_COIN_SET_A_PROBS = {{
    { 0.3749, 0.3749, 0.4551, 0.4167, 0.4582, 0.4582, 0.4011, 0.4281, 0.4023, 0.4023, 0.4265, 0.4167, 0.4225 },
    { 0.4327, 0.4327, 0.3931, 0.4169, 0.4322, 0.4322, 0.4205, 0.4189, 0.4364, 0.4364, 0.3958, 0.4143, 0.4224 },
    { 0.3809, 0.3809, 0.3792, 0.4273, 0.43, 0.43, 0.4138, 0.4154, 0.4116, 0.4116, 0.4234, 0.41, 0.4127 },
    { 0.4709, 0.4709, 0.4306, 0.4344, 0.4528, 0.4528, 0.4191, 0.4219, 0.4078, 0.4078, 0.4048, 0.4196, 0.4167 },
    { 0.3749, 0.3749, 0.3909, 0.429, 0.4582, 0.4582, 0.403, 0.4271, 0.4023, 0.4023, 0.4136, 0.4199, 0.4225 },
    { 0.4165, 0.4165, 0.4083, 0.415, 0.3952, 0.3952, 0.3925, 0.4142, 0.4097, 0.4097, 0.4022, 0.4049, 0.4087 },
    { 0.4526, 0.4526, 0.4371, 0.422, 0.4338, 0.4338, 0.4385, 0.4242, 0.415, 0.415, 0.4221, 0.4137, 0.4141 },
    { 0.4327, 0.4327, 0.4437, 0.4293, 0.4322, 0.4322, 0.4114, 0.4248, 0.4364, 0.4364, 0.4028, 0.4134, 0.4224 },
    { 0.4129, 0.4129, 0.4002, 0.4109, 0.4141, 0.4141, 0.4079, 0.4109, 0.4231, 0.4231, 0.4003, 0.4096, 0.4017 },
    { 0.3809, 0.3809, 0.4351, 0.4183, 0.43, 0.43, 0.4264, 0.4103, 0.4116, 0.4116, 0.4284, 0.4036, 0.4127 },
    { 0.4709, 0.4709, 0.4164, 0.4192, 0.4528, 0.4528, 0.4156, 0.4195, 0.4078, 0.4078, 0.4104, 0.416, 0.4167 },
    { 0.3952, 0.3952, 0.436, 0.4114, 0.4369, 0.4369, 0.4226, 0.4069, 0.403, 0.403, 0.4204, 0.4054, 0.4087 },
    { 0.4165, 0.4165, 0.413, 0.4189, 0.3952, 0.3952, 0.3998, 0.4049, 0.4097, 0.4097, 0.4261, 0.4098, 0.4087 },
    { 0.4526, 0.4526, 0.4173, 0.4276, 0.4338, 0.4338, 0.4227, 0.4159, 0.415, 0.415, 0.404, 0.4145, 0.4141 },
    { 0.4129, 0.4129, 0.3934, 0.4183, 0.4141, 0.4141, 0.4028, 0.408, 0.4231, 0.4231, 0.4217, 0.4045, 0.4017 },
    { 0.3952, 0.3952, 0.438, 0.4095, 0.4369, 0.4369, 0.406, 0.4128, 0.403, 0.403, 0.4254, 0.4088, 0.4087 },
}};
#elif defined (VARIANT_92)
constexpr inline std::array<std::array<double, MAX_WILD_COL_SIZE>, ESI_TOTAL_SIZE> EXTRA_SPIN_COIN_SET_A_PROBS = {{
    { 0.5244, 0.5244, 0.6052, 0.5833, 0.6238, 0.6238, 0.5696, 0.6038, 0.5773, 0.5773, 0.6012, 0.6001, 0.6064 },
    { 0.58, 0.58, 0.5452, 0.5838, 0.5983, 0.5983, 0.5879, 0.5948, 0.6105, 0.6105, 0.5712, 0.5975, 0.6061 },
    { 0.547, 0.547, 0.5486, 0.6091, 0.6119, 0.6119, 0.5971, 0.6073, 0.6039, 0.6039, 0.6155, 0.6081, 0.6107 },
    { 0.6186, 0.6186, 0.5826, 0.602, 0.619, 0.619, 0.5875, 0.5989, 0.5831, 0.5831, 0.5808, 0.6033, 0.6006 },
    { 0.5244, 0.5244, 0.5439, 0.5975, 0.6238, 0.6238, 0.5717, 0.604, 0.5773, 0.5773, 0.5888, 0.6034, 0.6064 },
    { 0.5809, 0.5809, 0.5771, 0.598, 0.5774, 0.5774, 0.5762, 0.6063, 0.6014, 0.6014, 0.5944, 0.603, 0.6068 },
    { 0.5994, 0.5994, 0.5896, 0.5905, 0.5999, 0.5999, 0.6066, 0.601, 0.5897, 0.5897, 0.5972, 0.5975, 0.598 },
    { 0.58, 0.58, 0.5965, 0.5986, 0.5983, 0.5983, 0.5799, 0.6019, 0.6105, 0.6105, 0.5777, 0.5971, 0.6061 },
    { 0.5787, 0.5787, 0.5697, 0.5945, 0.5964, 0.5964, 0.5915, 0.6038, 0.6151, 0.6151, 0.5935, 0.6081, 0.6003 },
    { 0.547, 0.547, 0.6036, 0.6021, 0.6119, 0.6119, 0.6098, 0.6032, 0.6039, 0.6039, 0.6205, 0.602, 0.6107 },
    { 0.6186, 0.6186, 0.5703, 0.5895, 0.619, 0.619, 0.5845, 0.5979, 0.5831, 0.5831, 0.5863, 0.6005, 0.6006 },
    { 0.5603, 0.5603, 0.6056, 0.5958, 0.6181, 0.6181, 0.6066, 0.5999, 0.5953, 0.5953, 0.6126, 0.6036, 0.6067 },
    { 0.5809, 0.5809, 0.5833, 0.6042, 0.5774, 0.5774, 0.5844, 0.5983, 0.6014, 0.6014, 0.618, 0.6074, 0.6068 },
    { 0.5994, 0.5994, 0.5723, 0.5987, 0.5999, 0.5999, 0.5915, 0.5943, 0.5897, 0.5897, 0.5793, 0.5985, 0.598 },
    { 0.5787, 0.5787, 0.5647, 0.6042, 0.5964, 0.5964, 0.5872, 0.602, 0.6151, 0.6151, 0.6142, 0.6032, 0.6003 },
    { 0.5603, 0.5603, 0.6095, 0.5966, 0.6181, 0.6181, 0.5906, 0.6068, 0.5953, 0.5953, 0.6175, 0.6073, 0.6067 },
}};
#else
#error "no variant defined"
#endif


inline const std::vector<std::vector<std::vector<int>>> EXTRA_SPIN_CR_REPLACEMENT_WS_COUNT_EXTRA_WILDS = {
    { /* level 1 */
        { 10000, 1731,  0 }, /* 1 CR -> WS { 0, 1, 2 } */
        { 10000, 8627,  118 }, /* 2 CR -> WS { 0, 1, 2 } */
        { 10000, 8438,  598 }, /* 3 CR -> WS { 0, 1, 2 } */
    },
    { // level 2
        { 10000, 2055, 0 },
        { 10000, 9742, 154 },
        { 10000, 7955, 427 },
    },
    { // level 3
        { 10000, 1611,   0 },
        { 10000, 10515, 97},
        { 10000, 8298,  57 },
    },
    { // level 4
        { 10000, 1611,  0 },
        { 10000, 10535, 0 },
        { 10000, 8381,  46 },
    },
};

inline const std::vector<std::vector<std::vector<int>>> EXTRA_SPIN_CR_REPLACEMENT_WS_COUNT_NO_EXTRA_WILDS = {
    { /* level 1 */
        { 10000, 1731,  0 }, /* 1 CR -> WS { 0, 1, 2 } */
        { 10000, 10702,  126 }, /* 2 CR -> WS { 0, 1, 2 } */
        { 10000, 11843,  930 }, /* 3 CR -> WS { 0, 1, 2 } */
    },
    { // level 2
        { 10000, 2055,  0 },
        { 10000, 12047, 126 },
        { 10000, 10975, 701 },
    },
    { // level 3
        { 10000, 1611,  0 },
        { 10000, 12954, 155 },
        { 10000, 11379, 73 },
    },
    { // level 4
        { 10000, 1611,  0 },
        { 10000, 12955, 0 },
        { 10000, 11458,  62 },
    },
};

// Original: array index = row number, value = mask pattern
constexpr inline std::array<uint8_t, ESI_TOTAL_SIZE> x = {
    0b0000,   // Row 0:  NO,  NO,  NO,  NO
    0b1000,   // Row 1:  YES, NO,  NO,  NO  (MORE_WILDS)
    0b0100,   // Row 2:  NO,  YES, NO,  NO  (REMOVE_LOWEST)
    0b0010,   // Row 3:  NO,  NO,  YES, NO  (MORE_COINS)
    0b0001,   // Row 4:  NO,  NO,  NO,  YES (EXTRA_2_SPINS)
    0b1100,   // Row 5:  YES, YES, NO,  NO
    0b1010,   // Row 6:  YES, NO,  YES, NO
    0b1001,   // Row 7:  YES, NO,  NO,  YES
    0b0110,   // Row 8:  NO,  YES, YES, NO
    0b0101,   // Row 9:  NO,  YES, NO,  YES
    0b0011,   // Row 10: NO,  NO,  YES, YES
    0b1110,   // Row 11: YES, YES, YES, NO
    0b1101,   // Row 12: YES, YES, NO,  YES
    0b1011,   // Row 13: YES, NO,  YES, YES
    0b0111,   // Row 14: NO,  YES, YES, YES
    0b1111    // Row 15: YES, YES, YES, YES
};

// Reverse: array index = mask pattern, value = row number
constexpr inline std::array<uint8_t, 16> maskToRow = {
    0,   // 0b0000 -> Row 0
    4,   // 0b0001 -> Row 4  (EXTRA_2_SPINS)
    3,   // 0b0010 -> Row 3  (MORE_COINS)
    10,  // 0b0011 -> Row 10
    2,   // 0b0100 -> Row 2  (REMOVE_LOWEST)
    9,   // 0b0101 -> Row 9
    8,   // 0b0110 -> Row 8
    14,  // 0b0111 -> Row 14
    1,   // 0b1000 -> Row 1  (MORE_WILDS)
    7,   // 0b1001 -> Row 7
    6,   // 0b1010 -> Row 6
    13,  // 0b1011 -> Row 13
    5,   // 0b1100 -> Row 5
    12,  // 0b1101 -> Row 12
    11,  // 0b1110 -> Row 11
    15   // 0b1111 -> Row 15
};

inline uint8_t getActualIndexFromMask (const uint8_t mask) {
    if (mask > 15)
        throw_error("Invalid mask, must be 0-15, got: ", mask);

    return maskToRow [mask];
}

inline uint8_t getEsInfoIdx (const std::vector<std::string>& activeFeatures, ExtraSpinInfo& esi) {
    uint8_t idx = 0;

    for (auto& af: activeFeatures) {
        uint8_t mask = 0;

        if (af == "MORE_WILDS") {
            idx |= MASK_MORE_WILDS;
            esi.extraWilds = true;
        } else if (af == "REMOVE_LOWEST") {
            idx |= MASK_REMOVE_LOWEST;
            esi.removeLowest = true;
        } else if (af == "MORE_COINS") {
            idx |= MASK_MORE_COINS;
            esi.moreCoins = true;
        } else if (af == "EXTRA_2_SPINS") {
            idx |= MASK_EXTRA_2_SPINS;
            esi.extra2Spins = true;
        } else if (af == "START_ON_LEVEL_2") {
            continue;
        } else {
            throw_error("invalid active feature in free/extra-spin, feat = ", af);
        }
    }

    const uint8_t actualIdx = getActualIndexFromMask (idx);
    if (actualIdx > 15)
        throw_error ("invalid index for extra-spin-info, idx > 15, means more than four active features, idx = ", idx);

    return actualIdx;
}

inline ExtraSpinInfo getExtraSpinInfo (const std::vector<std::string>& activeFeatures, const int wildsCollected) {
    if (wildsCollected < 0 || wildsCollected > 12)
        throw_error ("can't collect negative or > 12 wilds, #wilds = ", wildsCollected);

    ExtraSpinInfo esi;

    const uint8_t idx = getEsInfoIdx (activeFeatures, esi);
    esi.coinSetAProb  = EXTRA_SPIN_COIN_SET_A_PROBS [idx][wildsCollected];
    esi.id            = EXTRA_SPIN_IDS [idx][wildsCollected];
    esi.costX         = EXTRA_SPIN_COSTS [idx][wildsCollected];
    esi.featureMode  += (esi.id - 1);

    return esi;
}

struct EsBetConfig {
    std::string name;
    int mode;
    std::string type;
    double cost;
    double defaultMax;
    std::vector<double> allowed;
    double rtp = RTP;
    double maxWin;
    unsigned int size = 0;
};

inline std::vector<double> getAllowedBetsES(const double cost) {
    const size_t total = sizeof(bet_NORMAL) / sizeof(bet_NORMAL[0]);

    std::vector<double> abs;
    abs.reserve(total);

    for (size_t i = 0; i < total; i++)
        abs.push_back(bet_NORMAL[i] * cost);

    return abs;
}

inline const std::vector<EsBetConfig> ES_BET_CONFIGS = []() {
    std::vector<EsBetConfig> esBetConfigs;
    esBetConfigs.reserve(ESI_TOTAL_SIZE * MAX_WILD_COL_SIZE);

    for (int i = 0; i < ESI_TOTAL_SIZE; i++) {
        for (int j = 0; j < MAX_WILD_COL_SIZE; j++) {
            const auto id          = EXTRA_SPIN_IDS[i][j];
            const auto costX       = EXTRA_SPIN_COSTS[i][j];
            const auto featureMode = MODE_EXTRA_SPIN_START + id - 1;
            const auto allowed           = getAllowedBetsES(costX);
            unsigned int size            = static_cast<unsigned int>(allowed.size());

            esBetConfigs.push_back ({
                    "es" + std::to_string(id),
                    featureMode,
                    type_EXTRA_SPIN,
                    costX * CREDITS_PER_BET,
                    DEFAULT_MAX_BET_NORMAL_EUROCENTS * costX,
                    allowed,
                    RTP,
                    static_cast<double>(MAX_WIN_CAP) / costX,
                    size
                    });
        }
    }

    std::sort (esBetConfigs.begin(), esBetConfigs.end(), 
              [](const EsBetConfig& a, const EsBetConfig& b) {
                  return a.mode < b.mode;
              });

    return esBetConfigs;
}();
