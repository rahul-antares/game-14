#include <map>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "../../utils/cxxopts.h"
#include "../../utils/log.h"
#include "../../utils/exception.h"
#include "../../utils/exception.h"
#include "../../utils/hexdump.h"
#include "../../api/rng.h"

#define MAXCOUNT 1000000

void printMap (std::map<uint64_t, uint64_t>& map, uint64_t total) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        printf ("%4ld %6.3f%%\n", it->first, ((double) 100*it->second/total));
    }
}

auto parseOptions (int argc, char** argv) {
    cxxopts::Options options("simple-test", "A C++ driver for very simple test of RNG");
    options.add_options()
        ("i,iterations", "iterations", cxxopts::value<uint64_t>())
        ("h,help", "Print help")
        ;

    auto opts = options.parse(argc, argv);
    if (opts.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    return opts;
}

int main(int argc, char** argv) {
    uint64_t total = 0;
    std::map<uint64_t, uint64_t> map;
    try {
        auto opts           = parseOptions(argc, argv);
        uint64_t iterations = opts.count("iterations") ? opts["iterations"].as<uint64_t>():MAXCOUNT;

        auto r = new RngLib();
        r->reseed();

        for (uint64_t i = 0; i < iterations; i++) {
            double value = r->getDouble();

            int key = value <= 0.5 ? 0: 1;

            if (map.count(key) == 0)
                map.insert(std::make_pair(key, 0));

            map[key] += 1;
            total += 1;
        }

        printMap(map, total);
    }
    catch (const std::exception& ex) {
        LOGERROR("Error: %s\n", ex.what());
    }
    catch (...) {
        LOGERROR("Error: unknown exception caught in main\n");
    }

    return 0;
}

