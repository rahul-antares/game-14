#include "../utils/log.h"
#include "../utils/exception.h"
#include "../utils/hexdump.h"
#include "../lib/rand.h"
#include "./rng-local.h"

void RngProviderLocal::init([[ maybe_unused ]] RngOpts& rngOpts) {
    try { 
        rand.seed();
    }
    catch (...) {
        LOGERROR("RNG init failed\n");
        throw;
    }
}

void RngProviderLocal::getScaledV(uint64_V& in, uint64_V& out) {
    for (int i = 0; i < (int) in.size(); i++) {
        auto s = rand.getScaled(in[i]);
        out.push_back(s);
    }
}
