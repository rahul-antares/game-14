#include <jsoncpp/json/json.h>
#include <arpa/inet.h>
#include "../utils/log.h"
#include "../utils/exception.h"
#include "../utils/byteorder.h"
#include "../utils/hexdump.h"
#include "./rng-nats.h"

void RngProviderNats::init(RngOpts& rngOpts) {
    NatsOpts opts = rngOpts.nats;

    try { 
        rngCh                = opts.rngCh;
        url                  = opts.url;
        maxReconnect         = opts.maxReconnect;
        reconnectWait        = opts.reconnectWait;
        retryOnFailedConnect = opts.retryOnFailedConnect;

        natsConnect();
    }
    catch (...) {
        LOGERROR("RNG init failed\n");
        throw;
    }
}

// send binary data via Nats
void RngProviderNats::getScaledV(uint64_V& in, uint64_V& out) {
    throw std::logic_error("not yet implemented");

    natsMsg         *reply = NULL;
    Json::Value root, header, body, arrayInt;
    Json::StreamWriterBuilder builder;

    for (int i = 0; i < (int) in.size(); i++)
        arrayInt[i] = in[i];

    header["seq"]  = 0;
    header["req"]  = "scaled";
    body["data"]   = arrayInt;
    root["header"] = header;
    root["body"]   = body;

    const std::string message = Json::writeString(builder, root);

    natsStatus s = natsConnection_RequestString(&reply, conn, rngCh.c_str(), message.c_str(), 1000);
    if (s != NATS_OK)
        throw RNGEXC(natsStatus_GetText(s));

    parseVectorReply(reply, out);
    natsMsg_Destroy(reply);
}

// send binary data via Nats
uint64_t RngProviderNats::getScaled(uint64_t in) {
    natsMsg *reply = NULL;
    GetScaled_Req req;

    req.header.seq = htonll(0);
    req.header.req = 1;
    req.max        = htonl(in);

    natsStatus s = natsConnection_Request(&reply, conn, rngCh.c_str(), (const void*)&req, sizeof(req), 1000);
    if (s != NATS_OK)
        throw RNGEXC(std::string("natsConnection_RequestString :") + natsStatus_GetText(s));

    GetScaled_Response* response = (GetScaled_Response*) natsMsg_GetData(reply);

    if (response->header.error != 0)
        throw RNGEXC("error response from RNG server");

    uint64_t scaled = ntohl(response->s);
    natsMsg_Destroy(reply);
    return scaled;
}

void RngProviderNats::seed() {
    throw std::logic_error("not implemented yet");
}

void RngProviderNats::parseVectorReply(natsMsg* reply, uint64_V& out) {
    Json::Value root;
    Json::Reader reader;

    LOG("nats reply: %s - %.*s\n",
            natsMsg_GetSubject(reply),
            natsMsg_GetDataLength(reply),
            natsMsg_GetData(reply));

    reader.parse(natsMsg_GetData(reply), root);
    const Json::Value rValues = root["body"];
    if (!rValues.isArray())
        throw RNGEXC("unexpected response from nats: not an array");

    for (const auto& rValue : rValues) {
        uint64_t scaledValue = rValue["s"].asUInt();
        out.push_back(scaledValue);
    }
}

void RngProviderNats::natsConnect() {
    natsOptions *opts    = NULL;
    natsStatus s;
    const char* serverUrl = url.c_str();

    try {
        s = natsOptions_Create(&opts);  // Create options
        if (s != NATS_OK)
            throw RNGEXC(std::string("natsOptions_Create : ") + natsStatus_GetText(s));

        s = natsOptions_SetURL(opts, serverUrl);
        if (s != NATS_OK)
            throw RNGEXC(std::string("natsOptions_SetURL : ") + natsStatus_GetText(s));

        s = natsOptions_SetMaxReconnect(opts, maxReconnect);
        if (s != NATS_OK)
            throw RNGEXC(std::string("natsOptions_SetMaxReconnect : ") + natsStatus_GetText(s));

        s = natsOptions_SetReconnectWait(opts, reconnectWait);
        if (s != NATS_OK)
            throw RNGEXC(std::string("natsOptions_SetReconnectWait : ") + natsStatus_GetText(s));

        s = natsOptions_SetRetryOnFailedConnect(opts, retryOnFailedConnect, NULL, NULL);
        if (s != NATS_OK)
            throw RNGEXC(std::string("natsOptions_SetRetryOnFailedConnect :") + natsStatus_GetText(s));

        s = natsConnection_Connect(&conn, opts);
        if (s != NATS_OK)
            throw RNGEXC(std::string("natsConnection_Connect :") + natsStatus_GetText(s));

        natsOptions_Destroy(opts);
        LOGINFO("nats connection ok (url = %s)\n", serverUrl);
    }
    catch (const std::exception& e) {
        LOGERROR("nats connection to URL %s failed : %s\n", serverUrl, e.what());
        if (opts) natsOptions_Destroy(opts);
        throw;
    }
    catch (...) {
        if (opts) natsOptions_Destroy(opts);
        LOGERROR("nats connection to URL %s failed : unknown exception\n", serverUrl);
        throw;
    }
}
