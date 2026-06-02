#include <iterator>
#include <numeric>
#include <chrono>
#include <cmath>
#include "../game/data-defs.h"
#include "../utils/log.h"
#include "harness.h"

Harness::Harness () : config(game.getConfig()) {}

void Harness::initRNG (RngOpts opts) {
    game.initRNG(opts);
}

void Harness::run (HarnessInput input) {
    unsigned long long i, iterations = input.iterations;
    PlayOptions playOptions = input.playOptions;
    NextPlay nextOpts       = input.nextOpts;
    PlayResponse response;
    HarnessOutput output;
    int featureMode = playOptions.featureMode;

    this->clearBatchData ();

    /* Get start TS */
    this->batch.startTS = std::chrono::high_resolution_clock::now ();

    for (i = 0; i < iterations; i++) {
        response = game.play (playOptions);

        if (response.fsStatus.size()) {
            FsStatus fsStatus = response.fsStatus.back();
            while (fsStatus.level != -1 && fsStatus.status == INIT) {
                response = game.next (nextOpts, response);
                fsStatus = response.fsStatus.back();
            }
        }

        // this->validateResult (response);
        this->accumulateBatchData (response, featureMode);
    }

    /* Get end TS and calculate the run time*/
    this->batch.endTS = std::chrono::high_resolution_clock::now ();
    this->accumulateRunData ();

    return;
}

HarnessOutput Harness::getStats () {
    HarnessOutput output;

    output.batch              = batch;
    output.batch.rtp          = ((double) output.batch.totalWins * 100)/output.batch.totalBets;
    output.batch.baseLineRTP  = ((double) output.batch.baseLineWins * 100) / output.batch.totalBets;
    output.batch.baseCoinRTP  = ((double) output.batch.baseCoinWins * 100) / output.batch.totalBets;
    output.batch.respinRTP    = ((double) output.batch.respinWins * 100) / output.batch.totalBets;
    output.batch.freespinRTP  = ((double) output.batch.freespinWins * 100) / output.batch.totalBets;
    output.batch.baseSpin.rtp = ((double) output.batch.baseSpin.totalWins * 100)/output.batch.totalBets;
    output.batch.duration     = std::chrono::duration_cast<std::chrono::milliseconds>(batch.endTS - batch.startTS).count();

    output.total              = total;
    output.total.rtp          = ((double) output.total.totalWins * 100)/output.total.totalBets;
    output.total.baseLineRTP  = ((double) output.total.baseLineWins * 100) / output.total.totalBets;
    output.total.baseCoinRTP  = ((double) output.total.baseCoinWins * 100) / output.total.totalBets;
    output.total.respinRTP    = ((double) output.total.respinWins * 100) / output.total.totalBets;
    output.total.freespinRTP  = ((double) output.total.freespinWins * 100) / output.total.totalBets;
    output.total.baseSpin.rtp = ((double) output.total.baseSpin.totalWins * 100)/output.total.totalBets;

    return output;
}

PlayResponse Harness::getLastErrorResponse () {
    return lastErrorResponse;
}

void Harness::accumulateBatchData (PlayResponse& response, int featureMode) {
    int64_t winAmount         = response.winAmount;
    int64_t betAmount         = response.betAmount;
    int64_t baseCoinWinAmount = response.baseSpin.coinSymbolWins;
    int64_t baseLineWinAmount = response.baseSpin.baseWinAmount;
    int64_t freespinWinAmount = 0;
    int64_t respinWinAmount   = 0;

    batch.iterations     += 1;
    batch.totalBets      += betAmount;
    batch.totalWins      += winAmount;
    batch.winCount       += (winAmount ? 1:0);
    batch.respinOccCount += (response.baseSpin.reSpins.size() > 0 ? 1 : 0);

    batch.baseCoinWins  += baseCoinWinAmount;
    batch.baseLineWins  += baseLineWinAmount;
    batch.respinCount   += response.baseSpin.reSpins.size();
    batch.freespinCount += response.freeSpins.size();

    // add fs levels info
    if (response.fsStatus.size())
        batch.fsTotalCount++;

    int fsLevel = response.fsStatus.size() ? response.fsStatus.back().level: -1;
    if (fsLevel > 0) {
        batch.fsCounts[fsLevel - 1]++;
    }

    batch.baseSpin.totalWins += response.baseSpin.winAmount;

    // re-spin wins
    for (auto rs: response.baseSpin.reSpins) {
        respinWinAmount += rs.winAmount;
    }

    batch.respinWins += respinWinAmount;
    batch.respinWinCount += (respinWinAmount > 0 ? 1 : 0);

    // free-spin wins
    for (auto fs: response.freeSpins) {
        freespinWinAmount += fs.winAmount;
        batch.fsCoinWins  += fs.coinSymbolWins;
        batch.fsLineWins  += fs.baseWinAmount;

        for (auto rs: fs.reSpins) {
            freespinWinAmount  += rs.winAmount;
            batch.fsRespinWins += rs.winAmount;
        }
    }

    batch.freespinWins += freespinWinAmount;
    batch.freespinWinCount += (freespinWinAmount > 0 ? 1 : 0);

    const double multi = ((double) winAmount)/betAmount;
	if (multi >= config.betConfig[featureMode].maxWin)
        batch.maxWinCount++;

    if (multi > batch.maxWin)
        batch.maxWin = multi;

    auto& winMap = batch.winMap;
    if (winMap.count (multi) == 0)
        winMap.insert({ multi, 0 });

    winMap[multi] += 1;

    // Add reelset data
    auto& reelSetMap = batch.reelSetMap;
    if (reelSetMap.count (response.baseSpin.reelSet) == 0)
        reelSetMap.insert({ response.baseSpin.reelSet, 0 });

    reelSetMap[response.baseSpin.reelSet] += 1;
}

void Harness::accumulateRunData () {
    total.iterations         += batch.iterations;
    total.winCount           += batch.winCount;
    total.baseLineWins       += batch.baseLineWins;
    total.baseCoinWins       += batch.baseCoinWins;
    total.freespinWins       += batch.freespinWins;
    total.respinWins         += batch.respinWins;
    total.sbCount            += batch.sbCount;
    total.maxWinCount        += batch.maxWinCount;
    total.totalWins          += batch.totalWins;
    total.totalBets          += batch.totalBets;
    total.baseSpin.totalWins += batch.baseSpin.totalWins;
    total.fsTotalCount       += batch.fsTotalCount;
    total.freespinCount      += batch.freespinCount;
    total.respinCount        += batch.respinCount;
    total.respinOccCount     += batch.respinOccCount;

    // add fs levels info
    for (size_t i = 0; i < total.fsCounts.size(); i++)
        total.fsCounts[i] += batch.fsCounts[i];

    if (batch.maxWin > total.maxWin)
        total.maxWin = batch.maxWin;

    auto& winMap = total.winMap;
    for (auto it = batch.winMap.begin(); it != batch.winMap.end(); ++it) {
        double multi   = it->first;
        int64_t count = it->second;

        if (winMap.count (multi) == 0)
            winMap.insert({ multi, 0 });

        winMap[multi] += count;
    }

    total.mean = mean(winMap);
    total.variance = variance(winMap, total.mean);

    // Reelset data
    auto& reelSetMap = total.reelSetMap;
    for (auto it = batch.reelSetMap.begin(); it != batch.reelSetMap.end(); ++it) {
        std::string name = it->first;
        int64_t count   = it->second;

        if (reelSetMap.count (name) == 0)
            reelSetMap.insert({ name, 0 });

        reelSetMap[name] += count;
    }
}

void Harness::clearBatchData () {
    batch = {};
}

double Harness::mean (std::map<double, int64_t> winMap) {
    double totalWin = 0;
    int64_t totalCount = 0;

    for (auto it = winMap.begin(); it != winMap.end(); ++it) {
        double multi = it->first;
        int64_t count    = it->second;

        totalWin   += (multi * count);
        totalCount += count;
    }

    return ((double)totalWin)/((double)totalCount);
}

double Harness::variance(std::map<double, int64_t> winMap, double mean) {
    double sqDiff = 0;
    int64_t totalCount = 0;

    for (auto it = winMap.begin(); it != winMap.end(); ++it) {
        double multi = it->first;
        int64_t count    = it->second;

        sqDiff     += (pow(multi - mean, 2) * count);
        totalCount += count;
    }

    return ((double) sqDiff)/((double) totalCount);
}

void Harness::findROrMP (Grid grid, bool baseSpin, size_t spinCount, size_t genCount) {
    for (size_t i = 0; i < REEL_COUNT; i++) {
        for (size_t j = 0; j < GRID_HEIGHT; j++) {
            Symbol sym = grid[i][j];
        }
    }
}

void Harness::dumpPlayResponse (PlayResponse& response) {
    printf ("win amount = %ld\n", response.winAmount);
    printf ("bet amount = %ld\n", response.betAmount);
    printf ("ref bet amount = %ld\n", response.refBetAmount);
    printf ("    ---- base spin win = %ld\n", response.baseSpin.winAmount);

    /*
    for (ReSpin reSpin : response.reSpins) {
        printf ("        respin spin win amount = %d\n", reSpin.winAmount);
        unsigned long long total = 0;

        for (Winning winnings : respin.winnings) {
            total += winnings.winAmount;
            printf ("            respin win amount (from winnings)= %d (payline # %d, sym \"%s\")\n", winnings.winAmount, winnings.payLine, SymbolChar[winnings.symbol]);
        }

        printf ("            ---- respin win total = %llu\n", total);
    }
    */
}
