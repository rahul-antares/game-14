#pragma once
#include <stdlib.h>
#include <chrono>
#include <map>
#include "../game/game.h"
#include "../game/data-defs.h"

struct HarnessInput {
    unsigned long long iterations;
    PlayOptions        playOptions;
    NextPlay           nextOpts;
    bool               clearMaxWin = false;
};

struct SpinInfo {
    unsigned long long totalWins = 0;
    double rtp                   = 0;
};

struct DataCollection {
    unsigned long long iterations = 0;
    std::chrono::time_point<std::chrono::system_clock> startTS;
    std::chrono::time_point<std::chrono::system_clock> endTS;
    unsigned long long totalWins         = 0;
    unsigned long long totalBets         = 0;
    unsigned long long winCount          = 0;
    unsigned long long baseLineWins      = 0;
    unsigned long long baseCoinWins      = 0;
    unsigned long long respinWins        = 0;
    unsigned long long freespinWins      = 0;
    unsigned long long fsCoinWins        = 0;
    unsigned long long fsLineWins        = 0;
    unsigned long long fsRespinWins      = 0;
    unsigned long long freespinCount     = 0;
    unsigned long long freespinWinCount  = 0;
    unsigned long long respinCount       = 0;
    unsigned long long respinWinCount    = 0;
    unsigned long long respinOccCount    = 0;
    double maxWin                        = 0;
    long int maxWinCount                 = 0;
    double mean;
    double variance;
    double rtp                           = 0;
    double baseCoinRTP                   = 0;
    double baseLineRTP                   = 0;
    double respinRTP                     = 0;
    double freespinRTP                   = 0;
    unsigned long long duration          = 0;
    SpinInfo baseSpin;
    std::array<unsigned int, 4> fsCounts = {0};
    unsigned long fsTotalCount           = 0;
    unsigned long rsTotalCount           = 0;
    unsigned int sbCount                 = 0;
    std::map<double, long int> winMap;
    std::map<std::string, long int> reelSetMap;
};

struct HarnessOutput {
    DataCollection batch;
    DataCollection total;
};

class Harness {
    public:
        Harness ();
        void initRNG (RngOpts opts);
        void run (HarnessInput input);
        HarnessOutput getStats ();
        PlayResponse getLastErrorResponse ();

        Game game;

    private:
        // void validateResult (PlayResponse& response);
        void accumulateBatchData (PlayResponse& response, int featureMode);
        void accumulateRunData ();
        void clearBatchData ();
        void dumpPlayResponse (PlayResponse& response);
        void findROrMP (Grid grid, bool baseSpin, size_t spinCount, size_t genCount);
        double mean(std::map<double, long int> winMap);
        double variance(std::map<double, long int> winMap, double mean);

        GameConfig config;
        DataCollection total;
        DataCollection batch;
        PlayResponse lastErrorResponse;
};
