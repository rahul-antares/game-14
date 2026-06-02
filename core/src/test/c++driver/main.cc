#include <algorithm>
#include "./cxxopts.h"
#include "./main.h"
#include "./time.h"
#include "./locale.h"

int main (int argc, char* argv[]) {
    cxxopts::Options options("C++Driver", "A C++ driver for the game");
    options.add_options()
        ("b,betAmount", "bet amount", cxxopts::value<int>()->default_value("10"))
        ("f,featureMode", "feature mode", cxxopts::value<int>()->default_value("0"))
        ("h,help", "Print help")
        ;

    auto opts = options.parse(argc, argv);
    if (opts.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (!opts.count("betAmount")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    int betAmount   = opts["betAmount"].as<int>();
    int featureMode = opts["featureMode"].as<int>();


    PlayOptions input;

    input.betAmount    = betAmount;
    input.featureMode  = featureMode;

    Game game;
    PlayResponse result = game.play(input);

    return 0;
}
