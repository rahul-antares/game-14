#pragma once

#include <cstdint>
#include <vector>
#include "constants.h"
#include "data-defs.h"
#include "config.h"
#include "round.h"
#include "rng.h"

class Game {
    public:
        Game();
        GameConfig getConfig();
        PlayResponse init();
        void initRNG(RngOpts& rngOpts);
        void collectRands(PlayResponse& r);
        PlayResponse play(PlayOptions options);
        PlayResponse next(NextPlay next, PlayResponse prev);
        void applyCurrencyMultiplier(PlayResponse& r);
        void applyCurrMultiSpin (Spin& s, int64_t currMp);
        void dumpPlayResponse (PlayResponse r);
        void extraSpinsCondition (PlayResponse& resp);

        // other utility functions
        inline const char* getRNGLibType() { return rng.getType(); }
        void reseed();

        ~Game();

    protected:
        ReelSet* reelSet;
        RngLib rng;

        std::map<BetMode, std::map<int64_t, bool>> betmap;
        int64_t getRefBetBase (PlayOptions &options);
        int64_t calculateWins (const PlayResponse& playResponse);
        void createBetMap();
        void _play(PlayResponse& r, PlayOptions& opts, PayWindow& payWindow);

    private:
};
