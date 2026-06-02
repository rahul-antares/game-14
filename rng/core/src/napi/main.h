#pragma once

#include "../lib/rand.h"
#include <napi.h>

class RngModule : public Napi::ObjectWrap<RngModule> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        RngModule(const Napi::CallbackInfo& info);

    private:
        static Napi::FunctionReference constructor;
        Rand rand;

        void seed(const Napi::CallbackInfo& info);
        Napi::Value getVersion(const Napi::CallbackInfo& info);
        Napi::Value getScaledBinary(const Napi::CallbackInfo& info);
        Napi::Value getScaledV(const Napi::CallbackInfo& info);
        Napi::Value getScaled(const Napi::CallbackInfo& info);
};
