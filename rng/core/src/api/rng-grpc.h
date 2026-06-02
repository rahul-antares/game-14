#pragma once

#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <arpa/inet.h>
#include "rng.grpc.pb.h"
#include "./rng.h"
#include "../lib/rand.h"
#include "../utils/exception.h"

#define GRPC_DEFAULT_TIMEOUT 5

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using rng::RngService;
using rng::GetScaledRequest;
using rng::GetScaledResponse;

class GrpcClient {
    public:
        GrpcClient() : stub_(nullptr) {}

        void connect(const std::string& server_address, std::chrono::seconds timeout = std::chrono::seconds(GRPC_DEFAULT_TIMEOUT));
        uint64_t GetScaled(const uint64_t max);

    private:
        std::shared_ptr<Channel> channel_;
        std::unique_ptr<RngService::Stub> stub_;
};

class RngProviderGrpc : public RngProvider {
    public:
        RngProviderGrpc() : RngProvider("grpc") {}
        void init(RngOpts& opts);
        void seed();
        void getScaledV(uint64_V& in, uint64_V& out);
        uint64_t getScaled(uint64_t in);
    private:
        GrpcClient client;
        std::string host;
        uint16_t port;
};
