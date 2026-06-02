#pragma once

#include <string>
#include <vector>
#include <nats/nats.h>
#include <arpa/inet.h>
#include "./rng.h"
#include "../lib/rand.h"
#include "../utils/exception.h"

struct MsgHeader {
    uint64_t seq;
    uint8_t  req;
    uint8_t  error = 0;
    const uint16_t pad = htons(0xdead);
};

struct GetScaled_Req {
    MsgHeader header;
    uint64_t  max;
};

struct GetScaled_Response {
    MsgHeader header;
    uint64_t  s;
};

class RngProviderNats : public RngProvider {
    public:
        RngProviderNats() : RngProvider("nats") {}
        void init(RngOpts& opts);
        void getScaledV(uint64_V& in, uint64_V& out);
        uint64_t getScaled(uint64_t in);
        void seed();

    private:
        void natsConnect();
        natsConnection *conn = NULL;
        void parseVectorReply(natsMsg* reply, uint64_V& out);

        std::string rngCh;
        std::string url;
        uint64_t maxReconnect = 50;
        uint64_t reconnectWait = 500;
        bool retryOnFailedConnect = true;
};
