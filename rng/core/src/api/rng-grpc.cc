#include <jsoncpp/json/json.h>
#include <arpa/inet.h>
#include "../utils/log.h"
#include "../utils/exception.h"
#include "../utils/byteorder.h"
#include "../utils/hexdump.h"
#include "./rng-grpc.h"

void RngProviderGrpc::init(RngOpts& rngOpts) {
    GrpcOpts opts = rngOpts.grpc;

    try { 
        if (opts.host.empty())
            throw RNGEXC("host not provided");
        if (opts.port <= 0)
            throw RNGEXC("port not provided");

        host = opts.host;
        port = opts.port;

        client.connect(host + ":" + std::to_string(port));
    }
    catch (...) {
        LOGERROR("RNG init failed\n");
        throw;
    }
}

// send binary data via Nats
void RngProviderGrpc::getScaledV([[ maybe_unused ]] uint64_V& in, [[ maybe_unused ]] uint64_V& out) {
    throw std::logic_error("not yet implemented");
}

// send binary data via Nats
uint64_t RngProviderGrpc::getScaled(uint64_t in) {
    return client.GetScaled(in);
}

void RngProviderGrpc::seed() {
    throw std::logic_error("not implemented yet");
}

void GrpcClient::connect(const std::string& server_address, std::chrono::seconds timeout) {
    grpc::ChannelArguments channel_args;
    channel_args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);  // Ping every 10s
    channel_args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000); // Timeout in 5s
    channel_args.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
    channel_args.SetInt(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0); // Allow unlimited pings
    channel_args.SetInt(GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS, 5000);

    channel_ = grpc::CreateCustomChannel(server_address, grpc::InsecureChannelCredentials(), channel_args);
    if (!channel_)
        throw RNGEXC("Failed to create gRPC channel.");

    auto deadline = std::chrono::system_clock::now() + timeout;
    if (!channel_->WaitForConnected(deadline))
        throw RNGEXC("gRPC channel failed to connect within timeout.");

    stub_ = RngService::NewStub(channel_);
    if (!stub_)
        throw RNGEXC("Failed to create gRPC stub.");
}

uint64_t GrpcClient::GetScaled(const uint64_t max) {
    GetScaledRequest request;
    GetScaledResponse reply;
    ClientContext context;

    request.set_max(max);
    Status status = stub_->GetScaled(&context, request, &reply);
    if (!status.ok())
        throw RNGEXC("grpc error code " + status.error_code());

    return reply.scaled();
}

