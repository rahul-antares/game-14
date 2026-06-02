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
#include "../../utils/colors.h"
#include "../../utils/exception.h"
#include "../../utils/exception.h"
#include "../../utils/hexdump.h"
#include "../../api/rng.h"

auto parseOptions (int argc, char** argv) {
    cxxopts::Options options("simple-test", "A C++ driver to print the various versions of the RNG components");
    options.add_options()
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
    try {
        auto opts = parseOptions(argc, argv);
        auto r    = new RngLib();
        auto v    = r->getVersion();

        printf ("%16s = %s%s%s\n", "RNG lib version", ANSI_COLOR_GREEN, v.lib_v, ANSI_COLOR_RESET);
        printf ("%16s = %s%s%s\n", "OpenSSL version", ANSI_COLOR_GREEN, v.openssl_v, ANSI_COLOR_RESET);
        printf ("%16s = %s%s%s\n", "Algorithm", ANSI_COLOR_GREEN, v.algo, ANSI_COLOR_RESET);
        printf ("%16s = %s%s%s\n", "Build ts", ANSI_COLOR_GREEN, v.ts, ANSI_COLOR_RESET);
        printf ("%16s = %s%d%s\n", "Gcc version", ANSI_COLOR_GREEN, v.gcc_v, ANSI_COLOR_RESET);
        printf ("%16s = %s%lu%s\n", "C++ version", ANSI_COLOR_GREEN, v.cpp_v, ANSI_COLOR_RESET);
    }
    catch (const std::exception& ex) {
        LOGERROR("Error: %s\n", ex.what());
    }
    catch (...) {
        LOGERROR("Error: unknown exception caught in main\n");
    }

    return 0;
}

