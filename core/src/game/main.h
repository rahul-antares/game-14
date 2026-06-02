#pragma once

#include <napi.h>
#include "../game/game.h"
#include "../utils/data-conversion.h"

class GameWrapper : public Napi::ObjectWrap<GameWrapper> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        GameWrapper(const Napi::CallbackInfo& info);

    private:
        static Napi::FunctionReference constructor;
        Napi::Value getConfig(const Napi::CallbackInfo& info);
        Napi::Value init(const Napi::CallbackInfo& info);
        Napi::Value initRNG(const Napi::CallbackInfo& info);
        Napi::Value play(const Napi::CallbackInfo& info);
        Napi::Value next(const Napi::CallbackInfo& info);
        Napi::Value getRNGLibType(const Napi::CallbackInfo& info);
        Napi::Value cheats (const Napi::CallbackInfo& info);
        void reseed(const Napi::CallbackInfo& info);
        Game game;
};
