#include "./rng.h"
#if defined(__RNG__USE_LOCAL__)
#include "./rng-local.h"
#elif defined(__RNG__USE_NATS__)
#include "./rng-nats.h"
#elif defined(__RNG__USE_GRPC__)
#include "./rng-grpc.h"
#endif

void RngProvider::init([[ maybe_unused ]] RngOpts& rngOpts) {
    throw RNGEXC("parent - not implemented");
}

void RngProvider::getScaledV([[ maybe_unused ]] uint64_V& in, [[ maybe_unused ]] uint64_V& out) {
    throw RNGEXC("parent - not implemented");
}

void RngProvider::seed() {
    throw RNGEXC("parent - not implemented");
}

void RngProvider::getAndClearHistory([[ maybe_unused ]] std::vector<RandRecord>& h) {
    throw RNGEXC("parent - not implemented");
}

double RngProvider::getDouble() {
    throw RNGEXC("parent - not implemented");
}

uint64_t RngProvider::getScaled([[ maybe_unused ]] uint64_t in) {
    throw RNGEXC("parent - not implemented");
}

uint64_t RngProvider::getRaw() {
    throw RNGEXC("parent - not implemented");
}

RandVersion RngProvider::getVersion() {
    throw RNGEXC("parent - not implemented");
}

RngLib::RngLib() {
#if defined(__RNG__USE_LOCAL__)
    p = new RngProviderLocal();
#elif defined(__RNG__USE_NATS__)
    p = new RngProviderNats();
#elif defined(__RNG__USE_GRPC__)
    p = new RngProviderGrpc();
#else
#error "unknown RNG provider"
#endif
}

uint64_t RngLib::getWeightedIndex(const int* weights, const size_t length) {
    uint64_t totalWeight = 0;

    // Calculate total weight
    for (uint64_t i = 0; i < length; i++)
        totalWeight += weights[i];

    // If total weight is 0, throw
    if (totalWeight == 0)
        throw std::invalid_argument("sum of weights cannot be zero");

    // Generate a random number
    uint64_t randNum   = p->getScaled(totalWeight);
    uint64_t cumWeight = 0;

    for (uint64_t i = 0; i < length; i++) {
        cumWeight += weights[i];

        if (randNum < cumWeight)
            return i;
    }

    return length -1;
}

