#pragma once
#include <cstdint>
#include <vector>

enum BetMode {
    MODE_NORMAL = 0,
    MODE_ENHANCED,
    MODE_SB,
    MODE_FEATURE_BUY1,
    MODE_FEATURE_BUY2,
    MODE_FEATURE_BUY3,
    MODE_FEATURE_BUY4,
    MODE_FEATURE_BUY5,
    MODE_EXTRA_SPIN_START,
};

#define type_REGULAR     "regular"
#define type_BOOST       "boost"
#define type_SUPER_BOOST "superboost"
#define type_WILD_SPIN   "wildspin"
#define type_FEATURE     "feature"
#define type_EXTRA_SPIN  "extra-spin"

const int DEFAULT_MAX_BET_NORMAL_EUROCENTS   = 20000;
const int DEFAULT_MAX_BET_ENHANCED_EUROCENTS = 30000;
const int DEFAULT_MAX_BET_SB_EUROCENTS       = 50000;
const int DEFAULT_MAX_BET_FB1_EUROCENTS      = 1400000;
const int DEFAULT_MAX_BET_FB2_EUROCENTS      = 2400000;
const int DEFAULT_MAX_BET_FB3_EUROCENTS      = 3500000;
const int DEFAULT_MAX_BET_FB4_EUROCENTS      = 4500000;
const int DEFAULT_MAX_BET_FB5_EUROCENTS      = 5500000;

const int DEFAULT_BET_NORMAL_EUROCENTS   = 10;
const int DEFAULT_BET_ENHANCED_EUROCENTS = 15;
const int DEFAULT_BET_SB_EUROCENTS       = 25;
const int DEFAULT_BET_FB1_EUROCENTS      = 700;
const int DEFAULT_BET_FB2_EUROCENTS      = 1200;
const int DEFAULT_BET_FB3_EUROCENTS      = 1750;
const int DEFAULT_BET_FB4_EUROCENTS      = 2250;
const int DEFAULT_BET_FB5_EUROCENTS      = 2750;

const double MAX_WIN_REGULAR     = 5000;
const double MAX_WIN_BONUS_BOOST = 3333.3;
const double MAX_WIN_SB          = 2000;
const double MAX_WIN_FB1         = 71.4;
const double MAX_WIN_FB2         = 41.7;
const double MAX_WIN_FB3         = 28.6;
const double MAX_WIN_FB4         = 22.2;
const double MAX_WIN_FB5         = 18.2;

const int DEFAULT_MAX_BET_TIMES = 2000;

struct BetConfig {
    BetConfig(const char* name, int mode ,const char* type, int defaultMax, int cost, const int64_t bets[], unsigned int bets_size, double rtp, double maxWin):
        name(name),
        mode(mode),
        type(type),
        size(bets_size),
        cost(cost),
        defaultMax(defaultMax),
        allowed(bets),
        rtp(rtp),
        maxWin(maxWin)
    {}
    const char* name;
    const int mode;
    const char* type;
    const size_t size;
    const int  cost;
    const int  defaultMax;
    const int64_t* allowed;
    const double  rtp;
    const double  maxWin;
};

const int64_t bet_NORMAL[] = { 10, 20, 30, 50, 80, 100, 150, 200, 300, 500, 800, 1000, 1250, 1500, 2000, 3000, 5000, 8000, 10000, 12500, 15000, 20000, 30000, 50000, 80000, 100000, };

const int64_t bet_ENHANCED[] = { 15, 30, 45, 75, 120, 150, 225, 300, 450, 750, 1200, 1500, 1875, 2250, 3000, 4500, 7500, 12000, 15000, 18750, 22500, 30000, 45000, 75000, 120000, 150000, };

const int64_t bet_SB[] = { 25, 50, 75, 125, 200, 250, 375, 500, 750, 1250, 2000, 2500, 3125, 3750, 5000, 7500, 12500, 20000, 25000, 31250, 37500, 50000, 75000, 125000, 200000, 250000, };

const int64_t bet_FB1[] = { 700, 1400, 2100, 3500, 5600, 7000, 10500, 14000, 21000, 35000, 56000, 70000, 87500, 105000, 140000, 210000, 350000, 560000, 700000, 875000, 1050000, 1400000, 2100000, 3500000, 5600000, 7000000, };

const int64_t bet_FB2[] = { 1200, 2400, 3600, 6000, 9600, 12000, 18000, 24000, 36000, 60000, 96000, 120000, 150000, 180000, 240000, 360000, 600000, 960000, 1200000, 1500000, 1800000, 2400000, 3600000, 6000000, 9600000, 12000000, };

const int64_t bet_FB3[] = { 1750, 3500, 5250, 8750, 14000, 17500, 26250, 35000, 52500, 87500, 140000, 175000, 218750, 262500, 350000, 525000, 875000, 1400000, 1750000, 2187500, 2625000, 3500000, 5250000, 8750000, 14000000, 17500000, };

const int64_t bet_FB4[] = { 2250, 4500, 6750, 11250, 18000, 22500, 33750, 45000, 67500, 112500, 180000, 225000, 281250, 337500, 450000, 675000, 1125000, 1800000, 2250000, 2812500, 3375000, 4500000, 6750000, 11250000, 18000000, 22500000, };

const int64_t bet_FB5[] = { 2750, 5500, 8250, 13750, 22000, 27500, 41250, 55000, 82500, 137500, 220000, 275000, 343750, 412500, 550000, 825000, 1375000, 2200000, 2750000, 3437500, 4125000, 5500000, 8250000, 13750000, 22000000, 27500000, };

#if defined(VARIANT_96)
constexpr double RTP = 96.0;
#elif defined(VARIANT_94)
constexpr double RTP = 94.0;
#elif defined(VARIANT_92)
constexpr double RTP = 92.0;
#else
#error "unknown bet config for unknown variant"
#endif

const BetConfig BET_NORMAL (
    type_REGULAR,
    MODE_NORMAL,
    type_REGULAR,
    DEFAULT_MAX_BET_NORMAL_EUROCENTS,
    DEFAULT_BET_NORMAL_EUROCENTS,
    bet_NORMAL,
    sizeof bet_NORMAL/ sizeof * bet_NORMAL,
    RTP,
    MAX_WIN_REGULAR
);

const BetConfig BET_ENHANCED (
    "bonusboost",
    MODE_ENHANCED,
    type_BOOST,
    DEFAULT_MAX_BET_ENHANCED_EUROCENTS,
    DEFAULT_BET_ENHANCED_EUROCENTS,
    bet_ENHANCED,
    sizeof bet_ENHANCED/ sizeof * bet_ENHANCED,
    RTP,
    MAX_WIN_BONUS_BOOST
);

const BetConfig BET_SB (
    type_SUPER_BOOST,
    MODE_SB,
    type_SUPER_BOOST,
    DEFAULT_MAX_BET_SB_EUROCENTS,
    DEFAULT_BET_SB_EUROCENTS,
    bet_SB,
    sizeof bet_SB/ sizeof * bet_SB,
    RTP,
    MAX_WIN_SB
);

const BetConfig BET_FB1 (
    "fb1",
    MODE_FEATURE_BUY1,
    type_FEATURE,
    DEFAULT_MAX_BET_FB1_EUROCENTS,
    DEFAULT_BET_FB1_EUROCENTS,
    bet_FB1,
    sizeof bet_FB1/ sizeof * bet_FB1,
    RTP,
    MAX_WIN_FB1
);

const BetConfig BET_FB2 (
    "fb2",
    MODE_FEATURE_BUY2,
    type_FEATURE,
    DEFAULT_MAX_BET_FB2_EUROCENTS,
    DEFAULT_BET_FB2_EUROCENTS,
    bet_FB2,
    sizeof bet_FB2/ sizeof * bet_FB2,
    RTP,
    MAX_WIN_FB2
);

const BetConfig BET_FB3  (
    "fb3",
    MODE_FEATURE_BUY3,
    type_FEATURE,
    DEFAULT_MAX_BET_FB3_EUROCENTS,
    DEFAULT_BET_FB3_EUROCENTS,
    bet_FB3,
    sizeof bet_FB3/ sizeof * bet_FB3,
    RTP,
    MAX_WIN_FB3
);

const BetConfig BET_FB4  (
    "fb4",
    MODE_FEATURE_BUY4,
    type_FEATURE,
    DEFAULT_MAX_BET_FB4_EUROCENTS,
    DEFAULT_BET_FB4_EUROCENTS,
    bet_FB4,
    sizeof bet_FB4/ sizeof * bet_FB4,
    RTP,
    MAX_WIN_FB4
);
const BetConfig BET_FB5  (
    "fb5",
    MODE_FEATURE_BUY5,
    type_FEATURE,
    DEFAULT_MAX_BET_FB5_EUROCENTS,
    DEFAULT_BET_FB5_EUROCENTS,
    bet_FB5,
    sizeof bet_FB5/ sizeof * bet_FB5,
    RTP,
    MAX_WIN_FB5
);

const std::vector<BetConfig> ALL_BETS_CONFIG = {
    /* MODE_NORMAL          */ BET_NORMAL,
    /* MODE_ENHANCED        */ BET_ENHANCED,
    /* MODE_SUPERBOOST      */ BET_SB,
    /* MODE_FEATURE_BUY_FB1 */ BET_FB1,
    /* MODE_FEATURE_BUY_FB2 */ BET_FB2,
    /* MODE_FEATURE_BUY_FB3 */ BET_FB3,
    /* MODE_FEATURE_BUY_FB4 */ BET_FB4,
    /* MODE_FEATURE_BUY_FB5 */ BET_FB5,
};
