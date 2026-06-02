#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string.h>
#include "./main.h"
#include "../utils/log.h"
#include "../utils/error.h"
#include "../utils/extract.h"
#include "../game/constants.h"
#include "../game/data-defs.h"
#include "./cheats.h"
#include "napi.h"

Napi::FunctionReference GameWrapper::constructor;

// Napi Init functin to register exports from native addon
Napi::Object GameWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Game", {
            InstanceMethod("init",          &GameWrapper::init),
            InstanceMethod("initRNG",       &GameWrapper::initRNG),
            InstanceMethod("play",          &GameWrapper::play),
            InstanceMethod("next",          &GameWrapper::next),
            InstanceMethod("getConfig",     &GameWrapper::getConfig),
            InstanceMethod("getRNGLibType", &GameWrapper::getRNGLibType),
            InstanceMethod("reseed",        &GameWrapper::reseed),
            InstanceMethod("cheats",                  &GameWrapper::cheats),
            });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Game", func);
    return exports;
}

// Constructor method
GameWrapper::GameWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<GameWrapper>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
}

Napi::Value GameWrapper::cheats (const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Array c = Napi::Array::New(env, CheatsAll.size());

    for (auto i = 0; i < CheatsAll.size(); i++) {
        Napi::Object cheat = Napi::Object::New(env);

        cheat.Set("name",        Napi::String::New(env, CheatsAll[i].name));
        cheat.Set("type",        Napi::String::New(env, CheatsAll[i].type));
        cheat.Set("description", Napi::String::New(env, CheatsAll[i].description));
        cheat.Set("implMethod",  Napi::String::New(env, CheatsAll[i].implMethod));
        cheat.Set("implemented", Napi::Boolean::New(env, CheatsAll[i].implemented));

        c.Set(i, cheat);
    }

    return c;
}

Napi::Value GameWrapper::getConfig(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        GameConfig c = game.getConfig();

        return createConfigObject(env, c);
    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value GameWrapper::init(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {

        // If there is an option having a prevState
        if (info.Length() > 0 && info[0].IsObject()) {
            Napi::Object obj = info[0].As<Napi::Object>().ToObject();

            if (obj.Has("prevState")) {
                Napi::Value val = obj.Get("prevState");
                if (val.IsObject())
                    return val.As<Napi::Object>();
            }
        }

        PlayResponse r;

        do {
            r = game.play({
                    .featureMode        = 0,
                    .betAmount          = 10,
                    .currencyMultiplier = 1,
                    .action             = "spin"
                    });
        }
        while (r.winAmount != 0 || r.freeSpins.size() != 0);

        return playResponseToNAPI(env, r);
    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value GameWrapper::initRNG(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!info.Length())
            throw Napi::Error::New(env, "missing mandatory options");

        // There is at least one option
        if (!info[0].IsObject())
            throw Napi::Error::New(env, "invalid option type");

        Napi::Object obj = info[0].As<Napi::Object>().ToObject();

        RngOpts rngOpts = rngOptsToNative(env, obj, game.getRNGLibType());
        game.initRNG(rngOpts);

        return Napi::Object::New(env);

    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value GameWrapper::getRNGLibType(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        const char* s = game.getRNGLibType();
        Napi::Object o = Napi::Object::New(env);
        o.Set("type", Napi::String::New(env, s));

        return o;
    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

void GameWrapper::reseed(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        game.reseed();
    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value GameWrapper::play(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (!info.Length())
            throw Napi::Error::New(env, "missing mandatory options");

        // at least one object
        if (!info[0].IsObject())
            throw Napi::Error::New(env, "invalid option type");

        PlayOptions playOptions        = {};
        Napi::Object playOptionsObject = info[0].As<Napi::Object>().ToObject();
        bool buyFeature                = playOptionsObject.Get ("buyFeature").As<Napi::Boolean>();
        playOptions.buyFeature         = buyFeature ? 1 : 0;

        extract (playOptionsObject, "betAmount", playOptions.betAmount, true);
        extract (playOptionsObject, "featureMode", playOptions.featureMode, true);
        extract (playOptionsObject, "currencyMultiplier", playOptions.currencyMultiplier, true);
        if (playOptions.currencyMultiplier <= 0)
            throw Napi::Error::New(env, "invalid(negative/zero) currency-multplier value, expected a positive value");

        if (!playOptionsObject.Has("options") || !playOptionsObject.Get("options").IsObject())
            throw_error("missing options field in play params field");

        Napi::Object optionsObj   = playOptionsObject.Get("options").As<Napi::Object>().ToObject();
        extract (optionsObj, "extraSpin", playOptions.options.extraSpin, true);

        if (optionsObj.Has("prevState")) {
            Napi::Value val = optionsObj.Get("prevState");
            if (val.IsObject()) {
                Napi::Object prevState = val.As<Napi::Object>();
                if (prevState.IsEmpty())
                    throw Napi::Error::New(env, "missing round data in prevState");

                playOptions.options.prevState = napiPlayResponseToNative (prevState);
            } else {
                throw Napi::Error::New(env, "prevState should be a valid object");
            }
        } else if (playOptions.options.extraSpin) {
            throw_error ("extra spin requested but no previous state found");
        }

        Napi::Value cheatArg = info[1];
        PlayResponse playResponse;
        if (cheatArg.IsNull() || cheatArg.IsUndefined()) {
            // Call the base class "play" method
            playResponse  = game.play(playOptions);
            return playResponseToNAPI(env, playResponse);
        }

        Cheats cheats = parseCheats(env, cheatArg);

        playResponse = game.play(playOptions, cheats);
        return playResponseToNAPI(env, playResponse);

    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
        return {};
    }
}

Napi::Value GameWrapper::next(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (info.Length() != 3)
            throw Napi::Error::New(env, "missing mandatory parameters");

        PlayResponse playResponse;
        NextPlay nextOptions;
        Napi::Object nextOptObj = info[0].As<Napi::Object>().ToObject();
        nextOptions.gamble      = nextOptObj.Get("gamble").As<Napi::Boolean>();

        Napi::Object prevObj      = info[1].As<Napi::Object>();
        PlayResponse prevPlayResp = napiPlayResponseToNative(prevObj);

        Napi::Value cheatArg = info[2];
        if (cheatArg.IsNull() || cheatArg.IsUndefined()) {
            // Call the base class "play" method
            playResponse = game.next(nextOptions, prevPlayResp);
            return playResponseToNAPI(env, playResponse);
        }

        Cheats cheats = parseCheats(env, cheatArg);
        playResponse  = game.next(nextOptions, prevPlayResp, cheats);

        return playResponseToNAPI(env, playResponse);

    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
        return {};
    }
}

// exports 
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return GameWrapper::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
