#pragma once

#include <string>
#include <vector>
#include <arpa/inet.h>
#include "../lib/rand.h"
#include "../utils/exception.h"

struct NatsOpts {
    std::string             url;
    std::string             rngCh;
    std::string             rngChJson;
    int64_t                 timeout = 2000;
    std::string             name;
    bool                    verbose = false;
    bool                    allowReconnect = true;
    bool                    retryOnFailedConnect = true;
    bool                    secure;
    bool                    tlsHandshakeFirst;
    uint64_t                maxReconnect = 50;
    uint64_t                reconnectWait = 500;

    std::string             user;
    std::string             password;
    std::string             token;

    int64_t                 pingInterval = 5000;
    int                     maxPingsOut = 5;
    int                     maxPendingMsgs;
    int64_t                 maxPendingBytes;

    // Custom inbox prefix
    char *inboxPfx;
};

struct GrpcOpts {
    std::string  host;
    uint16_t     port;
};

struct RngOpts {
    NatsOpts nats;
    GrpcOpts grpc;
};

using uint64_V = std::vector<uint64_t>;

class RngProvider {
    public:
        RngProvider(const char* type) : type(type) {};
        inline const char* getType() { return type; };
        virtual void init(RngOpts& opts);
        virtual void seed();
        virtual void getScaledV(uint64_V& in, uint64_V& out);
        virtual uint64_t getScaled(uint64_t in);
        virtual double getDouble();
        virtual uint64_t getRaw();
        virtual RandVersion getVersion();
        virtual void getAndClearHistory(std::vector<RandRecord>& h);
        virtual ~RngProvider() = default;

    protected:
        const char* type;
};

class RngLib {
    public:
        RngLib();
        inline const char* getType() { return p->getType(); };
        inline void init(RngOpts& opts) { p->init(opts); };
        inline void reseed() { p->seed(); };
        inline RandVersion getVersion() { return p->getVersion(); };
        inline void getScaledV(uint64_V& in, uint64_V& out) { p->getScaledV(in, out); };
        inline uint64_t getScaled(uint64_t in) { return p->getScaled(in); };
        inline double getDouble() { return p->getDouble(); };
        inline uint64_t getRaw() { return p->getRaw(); };
        uint64_t getWeightedIndex(const int* weights, const size_t size);
        inline void getAndClearHistory(std::vector<RandRecord>& h) { p->getAndClearHistory(h); }
    private:
        RngProvider* p;
};
