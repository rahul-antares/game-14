#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "build/rng.grpc.pb.h"
#include "../../../core/src/utils/log.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using rng::RngService;
using rng::GetScaledRequest;
using rng::GetScaledResponse;

class GrpcClient {
    public:
        GrpcClient() : stub_(nullptr) {}

        void connect(const std::string& server_address, std::chrono::seconds timeout = std::chrono::seconds(10));
        uint32_t GetScaled(const uint32_t max);

    private:
        std::shared_ptr<Channel> channel_;
        std::unique_ptr<RngService::Stub> stub_;
};

void GrpcClient::connect(const std::string& server_address, std::chrono::seconds timeout) {
    channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    if (!channel_)
        throw std::logic_error("Failed to create gRPC channel.");

    auto deadline = std::chrono::system_clock::now() + timeout;
    if (!channel_->WaitForConnected(deadline))
        throw std::logic_error("gRPC channel failed to connect within timeout.");

    stub_ = RngService::NewStub(channel_);
    if (!stub_)
        throw std::logic_error("Failed to create gRPC stub.");
}

uint32_t GrpcClient::GetScaled(const uint32_t max) {
    GetScaledRequest request;
    GetScaledResponse reply;
    ClientContext context;

    request.set_max(max);
    Status status = stub_->GetScaled(&context, request, &reply);
    if (!status.ok())
        throw std::logic_error("grpc error code " + status.error_code());

    return reply.scaled();
}

int main() {
    GrpcClient client;

    try {
        client.connect("localhost:50051");
        uint32_t a = client.GetScaled(100);
        printf ("grpc response = %d\n", a);
        return 0;
    }
    catch (std::exception& e) {
        LOGERROR("ERROR: %s\n", e.what());
    }
    catch(...) {
        LOGERROR("unknown error\n");
    }

    return 1;
}
