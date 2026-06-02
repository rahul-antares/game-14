#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <atomic>
#include <sys/time.h>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "build/rng.grpc.pb.h"
#include "../../core/src/lib/rand.h"
#include "../../core/src/utils/log.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using rng::RngService;
using rng::GetScaledRequest;
using rng::GetScaledResponse;

Rand _rand;

class GetScaledService final : public RngService::Service {
    Status GetScaled(
            ServerContext* context,
            const GetScaledRequest* request,
            GetScaledResponse* reply) override 
    {
        uint32_t max = request->max();
        uint32_t out = _rand.getScaled(max);
        LOG("max=%u s=%u\n", max, out);
        reply->set_scaled(out);
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("[::]:50051");
    GetScaledService service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.SetSyncServerOption(grpc::ServerBuilder::MAX_POLLERS, 10);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);

    builder.RegisterService(&service);
    
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

std::atomic<bool> stopTimer(false);

void periodicTimer(int interval) {
    printf ("-------------------------------------");
    /*
    while (!stopTimer) {
        std::this_thread::sleep_for(std::chrono::seconds(interval));
        std::cout << "Tick: " << interval << " seconds passed\n";
    }
    */
}

#define PERIOD_S 1
int setPeriodicTimer() {
    std::thread timerThread(periodicTimer, PERIOD_S);

    printf ("timer set for %d seconds periodic\n", PERIOD_S);
    return 0;
}

int main() {
    try {
        setPeriodicTimer();
        RunServer();
        stopTimer = true;
    }
    catch (const std::exception& e) {
        fprintf(stderr, "exception : %s\n", e.what());
    }
    catch (...) {
        fprintf(stderr, "unknown exception\n");
    }

    return 0;
}

