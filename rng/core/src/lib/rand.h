#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>
#include <cmath>

#define SEED_SIZE 256
#define HISTORY_SIZE 128
#define __lib_VERSION__ "2.14"

struct RandRecord {
    uint64_t bStream; // raw random bitstream
    uint64_t scaled;  // scaled
    uint64_t max;     // input max
    double   real;    // corresponding random number between 0 & 1
};

struct RandVersion {
    const char* lib_v = __lib_VERSION__;
    const char* openssl_v;
    const char* algo;
    const char* ts = __TIMESTAMP__;
    const int gcc_v = __GNUC__;
    const long int cpp_v = __cplusplus;
};

class Rand {
    public:
        Rand() {
            // to avoid re-allocations
            history.reserve(HISTORY_SIZE);
        };
        RandVersion getVersion();
        void seed(void);
        uint64_t getScaled(uint64_t max);
        double getDouble();
        uint64_t getRaw();
        void getAndClearHistory(std::vector<RandRecord>& h);

    private:
        std::vector<RandRecord> history;
        void pushToHistory(RandRecord);
};
