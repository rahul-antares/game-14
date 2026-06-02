#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "../utils/log.h"
#include "../utils/exception.h"
#include "../utils/exception.h"
#include "../utils/hexdump.h"
#include "./rand.h"
#include "./date.h"

RandVersion Rand::getVersion() {
    RandVersion v;

    v.openssl_v        = OpenSSL_version(OPENSSL_FULL_VERSION_STRING);
    EVP_RAND_CTX* ctx  = RAND_get0_private(NULL);
    EVP_RAND* evp_rand = EVP_RAND_CTX_get0_rand(ctx);
    v.algo             = evp_rand ? EVP_RAND_get0_name(evp_rand) : "unknown";

    return v;
}

void Rand::seed(void) {
    int fd = open("/dev/urandom", O_RDONLY);

    if (fd < 0)
        throw RNGEXC("unable to open \"/dev/urandom\"");

    char seed_data[SEED_SIZE];
    int n = read(fd, seed_data, sizeof(seed_data));
    if (n < 0)
        throw RNGEXC("data read error from \"/dev/unrandom\"");

    close(fd);

    RAND_seed(seed_data, sizeof(seed_data));
    LOG ("[%s] RNG reseeded\n", get_time().c_str());
}

#if 0
#include <random>
uint64_t Rand::getScaled(uint64_t max) {
    // Create a random number engine and seed it with a random device
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Define a uniform distribution for unsigned integers between 0 and max-1
    std::uniform_int_distribution<unsigned int> distribution(0, max - 1);

    // Generate and return a random number
    return distribution(gen);
}
#else
uint64_t Rand::getScaled(uint64_t max) {
    uint64_t rand_int = getRaw();

    constexpr double scaleFactor = 1.0 / (static_cast<double>(UINT64_MAX) + 1.0);
    double x                     = ((double) rand_int) * scaleFactor;
    uint64_t scaled              = static_cast<uint64_t>((double) max * x);

    pushToHistory({ 
            .bStream = rand_int,
            .scaled  = scaled,
            .max     = max,
            .real    = x
            });

    return scaled;
}
#endif

void Rand::pushToHistory(RandRecord record) {
    if (history.size() >= HISTORY_SIZE)
        return;

    history.push_back(record);
}

void Rand::getAndClearHistory(std::vector<RandRecord>& h) {
    h = history;
    history.clear();
}

double Rand::getDouble() {
    uint64_t rand_int = getRaw();

    double x = ((double) rand_int) / (static_cast<double>(UINT64_MAX) + 1.0);

    pushToHistory({ 
            .bStream = rand_int,
            .scaled  = 0,
            .max     = 0,
            .real    = x
            });

    return x;
}

uint64_t Rand::getRaw() {
    uint64_t rand_int;

    if (RAND_priv_bytes(
                reinterpret_cast<unsigned char*>(&rand_int),
                sizeof(rand_int)) == 1
            )
        return rand_int;

    // Else it has failed
    unsigned long err = ERR_get_error();
    fprintf(stderr, "OpenSSL error: %s\n", err != 0? ERR_error_string(err, NULL): "unknown error");

    if (RAND_poll() != 1 && RAND_priv_bytes(
                reinterpret_cast<unsigned char*>(&rand_int),
                sizeof(rand_int)) == 1) {
        // IMPORTANT: do not change the string below as it used
        // upstream to take logic decisions
        throw RNGEXC("UNABLETOGENRANDOM");
    }

    return rand_int;
}
