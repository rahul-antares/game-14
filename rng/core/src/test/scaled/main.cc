#include <cstdint>
#include <cmath>
#include <cstring>
#include <exception>
#include <stdio.h>
#include <fcntl.h>
#include <tuple>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "../../utils/cxxopts.h"
#include "../../utils/log.h"
#include "../../utils/colors.h"
#include "../../utils/hexdump.h"
#include "../../api/rng.h"

uint64_t parse_size(const std::string& input);

std::tuple<uint64_t, uint64_t> parseOptions (int argc, char** argv) {
    cxxopts::Options options("scaled", "A basic C++ driver to generate scaled random values");
    options.add_options()
        ("m,max",       "Max value of the range (Required)", cxxopts::value<uint64_t>())
        ("n,num",       "Number of values to generate (suffixes = <none>, K, M) (Required)", cxxopts::value<std::string>())
        ("h,help",      "Print help")
    ;

    auto opts = options.parse(argc, argv);
    if (opts.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (
        opts.count("num") == 0 &&
        opts.count("max") == 0
    ) {
        std::cout << ANSI_COLOR_RED << "ERROR:  -n and -m options must have a non-zero value\n"<< ANSI_COLOR_RESET << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (
        opts.count("num") != 0 &&
        opts.count("size") != 0
    ) {
        std::cout << ANSI_COLOR_RED << "ERROR: cannot specify -n and -s at the same time\n"<< ANSI_COLOR_RESET << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    auto numStr = opts["num"].as<std::string>();
    uint64_t num    =  parse_size(numStr);
    uint64_t max    = opts["max"].as<uint64_t>();

    return std::make_tuple(num, max);
}

void generateBatch(RngLib* r, uint64_t num, uint64_t maxValue) {
    for (uint64_t i = 0; i < num; i++) {
        uint64_t value = r->getScaled(maxValue);
        std::cout << value << std::endl;
    }
}


int main(int argc, char** argv) {
    try {
        auto [num, max] = parseOptions(argc, argv);
        auto r = new RngLib();
        r->reseed();
        generateBatch(r, num, max);
    }
    catch (std::exception& e) {
        fprintf (stderr, "%sError: %s%s\n", ANSI_COLOR_RED, e.what(), ANSI_COLOR_RESET);
    }
    catch (...) {
        fprintf (stderr, "%sError: unknown error%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
    }

    return 0;
}

