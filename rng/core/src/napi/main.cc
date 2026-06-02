#include <iostream>
#include <cstddef>
#include <string.h>
#include <arpa/inet.h>
#include "../utils/log.h"
#include "../utils/hexdump.h"
#include "./main.h"
#include "./conversion.h"
#include "../api/rng.h"
#include "../api/rng-nats.h"
#include "../utils/exception.h"
#include "../utils/byteorder.h"
#include "napi.h"

Napi::FunctionReference RngModule::constructor;

// Napi Init function to register exports from native addon
Napi::Object RngModule::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "RngModule", {
            InstanceMethod("seed",            &RngModule::seed),
            InstanceMethod("getVersion",      &RngModule::getVersion),
            InstanceMethod("getScaled",       &RngModule::getScaled),
            InstanceMethod("getScaledV",      &RngModule::getScaledV),
            InstanceMethod("getScaledBinary", &RngModule::getScaledBinary),
            });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("RngModule", func);
    return exports;
}

// Constructor method
RngModule::RngModule(const Napi::CallbackInfo& info) : Napi::ObjectWrap<RngModule>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
}

// getConfig static method
void RngModule::seed(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        rand.seed();
    } catch (const std::exception& ex) {
        LOGERROR("[napi::seed] error %s\n", ex.what());
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        LOGERROR("[napi::seed] unknown error\n");
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value RngModule::getVersion(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        RandVersion v       = rand.getVersion();
        Napi::Object verObj = Napi::Object::New(env);

        verObj.Set("lib_v", Napi::String::New(env, v.lib_v));
        verObj.Set("openssl_v", Napi::String::New(env, v.openssl_v));
        verObj.Set("algo", Napi::String::New(env, v.algo));
        verObj.Set("ts", Napi::String::New(env, v.ts));
        verObj.Set("gcc_v", Napi::Number::New(env, v.gcc_v));
        verObj.Set("cpp_v", Napi::Number::New(env, v.cpp_v));

        return verObj;
    } catch (const std::exception& ex) {
        LOGERROR("[napi::getVersion] error %s\n", ex.what());
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        LOGERROR("[napi::getVersion] unknown error\n");
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value RngModule::getScaledV(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (info.Length() < 1)
            throw RNGEXC("too few arguments");

        // Object has the form => [ <value1>, <value2> ... ]
        Napi::Array opts = info[0].As<Napi::Array>();
        std::vector<uint32_t> vector;

        for (uint32_t i = 0; i < opts.Length(); i++) {
            uint32_t scale = opts.Get(i).As<Napi::Number>().Int32Value();
            uint32_t val;
            val = rand.getScaled(scale);
            vector.push_back(val);
        }

        return convertVectoPairToNapi (env, vector);

    } catch (const std::exception& ex) {
        LOGERROR("[napi::getScaledV] error %s\n", ex.what());
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        LOGERROR("[napi::getScaledV] unknown error\n");
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value RngModule::getScaledBinary(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (info.Length() < 1)
            throw RNGEXC("too few arguments");

        Napi::Object input = info[0].As<Napi::Object>();
        if (!input.IsBuffer())
            throw RNGEXC("input is not a buffer");

        Napi::Buffer<char> _b  = input.As<Napi::Buffer<char>>();
        GetScaled_Req* payload = (GetScaled_Req*) _b.Data();

        uint32_t max = ntohl(payload->max);
        auto s = rand.getScaled(max);

        GetScaled_Response* response = new GetScaled_Response();
        std::memcpy((void*) response, (void*)payload, sizeof(MsgHeader));
        response->s = htonl(s);

        Napi::ArrayBuffer p = Napi::ArrayBuffer::New(
                env,
                (void*)response,
                sizeof(*response),
                [response](Napi::Env, void* data) { delete response; }
                );

        return p;

    } catch (const std::exception& ex) {
        LOGERROR("[napi::getScaledBinary] error %s\n", ex.what());
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        LOGERROR("[napi::getScaledBinary] unknown error\n");
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}


Napi::Value RngModule::getScaled(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    uint32_t val;

    try {
        if (info.Length() < 1)
            throw RNGEXC("too few arguments");

        // Object has the form => <value>
        Napi::Object pObj = info[0].As<Napi::Object>();
        uint32_t scale    = pObj.As<Napi::Number>().Int32Value();
        val               = rand.getScaled(scale);

        Napi::Object p = Napi::Object::New(env);
        p.Set("s", Napi::Number::New(env, val));

        return p;

    } catch (const std::exception& ex) {
        LOGERROR("[napi::getScaled] error %s\n", ex.what());
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        LOGERROR("[napi::getScaled] unknown error\n");
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}


// exports 
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return RngModule::Init(env, exports);
}


NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
