#include <algorithm>
#include "./cxxopts.h"
#include "./main.h"
#include "./time.h"
#include "./locale.h"

int main (int argc, char* argv[]) {
    cxxopts::Options options("C++Driver", "A C++ driver for the harness/game");
    options.add_options()
        ("i,iterations", "iterations", cxxopts::value<ull>())
        ("b,betAmount", "bet amount", cxxopts::value<int>()->default_value("10"))
        ("batch-size", "batch size", cxxopts::value<ull>())
        ("h,help", "Print help")
        ;

    auto opts = options.parse(argc, argv);
    if (opts.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    ull iterations   = opts["iterations"].as<ull>();
    ull batchSize    = opts["batch-size"].as<ull>();
    int betAmount    = opts["betAmount"].as<int>();


    Harness harness;
    HarnessInput input;

    input.playOptions.betAmount    = betAmount;
    input.playOptions.featureMode  = 0;

    ull remaining = iterations;
    do {

        input.iterations = std::min (remaining, batchSize);

        if (input.iterations)
            harness.run (input);

        remaining -= input.iterations;
        //showStats (harness, true);
    } while (remaining > 0);

    //HarnessOutput output = harness.getStats ();

    return 0;
}

void showStats (Harness& harness, bool showBatch) {
    HarnessOutput output = harness.getStats();
    DataCollection data  = showBatch? output.batch: output.total;

    printf ("+%12s [%s]: RTP=%6.2f%%\n",
            formatInt(data.iterations).c_str(),
            formatMilliseconds(data.duration).c_str(),
            data.rtp
            );
}
