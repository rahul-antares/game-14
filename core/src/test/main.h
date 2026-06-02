#pragma once

#include <napi.h>
#include "../utils/data-conversion.h"
#include "harness.h"

class HarnessNapi : public Napi::ObjectWrap<HarnessNapi> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        HarnessNapi(const Napi::CallbackInfo& info);

    private:
        static Napi::FunctionReference constructor;
        Napi::Value initRNG(const Napi::CallbackInfo& info);
        Napi::Value getRNGLibType(const Napi::CallbackInfo& info);
        Napi::Value run(const Napi::CallbackInfo& info);
        Napi::Value getStats(const Napi::CallbackInfo& info);
        Napi::Value getLastErrorResponse(const Napi::CallbackInfo& info);

        Harness harness;
};

Napi::Object harnessOutputToNapiObject(Napi::Env env, const HarnessOutput& data);
