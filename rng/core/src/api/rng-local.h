#pragma once

#include <string>
#include <vector>
#include <nats/nats.h>
#include <arpa/inet.h>
#include "./rng.h"
#include "../lib/rand.h"
#include "../utils/exception.h"

class RngProviderLocal : public RngProvider {
    public:
        RngProviderLocal() : RngProvider("local") {};
        ~RngProviderLocal() = default;
        void init(RngOpts& opts);
        inline void seed() { rand.seed(); }
        void getScaledV(uint64_V& in, uint64_V& out);
        inline RandVersion getVersion() { return rand.getVersion(); }
        inline uint64_t getRaw() { return rand.getRaw(); }
        inline double getDouble() { return rand.getDouble(); }
        inline uint64_t getScaled(uint64_t in) { return rand.getScaled(in); }
        inline void getAndClearHistory(std::vector<RandRecord>& h) { rand.getAndClearHistory(h); }

    private:
        Rand rand;
};
