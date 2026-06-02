#include <iostream>
#include <cstddef>
#include <string.h>
#include "main.h"
#include "../utils/log.h"

Napi::FunctionReference HarnessNapi::constructor;

// Napi Init functin to register exports from native addon
Napi::Object HarnessNapi::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Harness", {
            InstanceMethod("initRNG",              &HarnessNapi::initRNG),
            InstanceMethod("run",                  &HarnessNapi::run),
            InstanceMethod("getStats",             &HarnessNapi::getStats),
            InstanceMethod("getLastErrorResponse", &HarnessNapi::getLastErrorResponse),
            InstanceMethod("getRNGLibType",        &HarnessNapi::getRNGLibType),
            });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Harness", func);
    return exports;
}

// Constructor method
HarnessNapi::HarnessNapi(const Napi::CallbackInfo& info) : Napi::ObjectWrap<HarnessNapi>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
}

Napi::Value HarnessNapi::getRNGLibType(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        const char* s = harness.game.getRNGLibType();
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

Napi::Value HarnessNapi::initRNG(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    try {
        if (!info.Length())
            throw Napi::Error::New(env, "missing mandatory options");

        // There is at least one option
        if (!info[0].IsObject())
            throw Napi::Error::New(env, "invalid option type");

        Napi::Object obj = info[0].As<Napi::Object>().ToObject();

        RngOpts rngOpts = rngOptsToNative(env, obj, harness.game.getRNGLibType());
        harness.initRNG(rngOpts);

        return Napi::Object::New(env);

    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
    }
}

Napi::Value HarnessNapi::run(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (!info.Length())
            throw Napi::Error::New(env, "missing mandatory options");

        // There is at least one option
        if (!info[0].IsObject())
            throw Napi::Error::New(env, "invalid option type");

        Napi::Object optionsObject = info[0].As<Napi::Object>().ToObject();

        HarnessInput input;

        Napi::Object playOptsObj = optionsObject.Get("playOptions").As<Napi::Object>();
        PlayOptions playOptions;

        playOptions.betAmount          = playOptsObj.Get("betAmount").As<Napi::Number>().Int64Value();
        playOptions.featureMode        = playOptsObj.Get("featureMode").As<Napi::Number>().Int64Value();
        playOptions.buyFeature         = playOptsObj.Get("buyFeature").As<Napi::Number>().Int64Value();
        playOptions.currencyMultiplier = playOptsObj.Get("currencyMultiplier").As<Napi::Number>().Int64Value();

        Napi::Object nextOptsObj = optionsObject.Get("nextOpts").As<Napi::Object>();
        input.nextOpts.gamble    = nextOptsObj.Get("gamble").As<Napi::Boolean>();

        input.playOptions = playOptions;
        input.iterations  = optionsObject.Get("iterations").As<Napi::Number>().Int64Value();
        input.clearMaxWin = optionsObject.Get("clearMaxWin").As<Napi::Boolean>();

        harness.run(input);
        return {};

    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
        return {};
    }
}

Napi::Value HarnessNapi::getStats(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        HarnessOutput output = harness.getStats ();

        return (harnessOutputToNapiObject (env, output));
    } catch (const std::exception& ex) {
        // Catch standard C++ exceptions
        throw Napi::Error::New(env, ex.what());
    } catch (...) {
        // Catch all other exceptions
        throw Napi::Error::New(env,"An unknown error occurred");
        return {};
    }
}

Napi::Value HarnessNapi::getLastErrorResponse(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        PlayResponse output = harness.getLastErrorResponse ();

        return (playResponseToNAPI (env, output));
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
    return HarnessNapi::Init(env, exports);
}

Napi::Object convertFsCounts(Napi::Env env, const std::array<unsigned int, 4>& data) {
    Napi::Array array = Napi::Array::New(env);

    for (size_t i = 0; i < data.size(); i++) {
        array.Set(i, Napi::Number::New(env, data[i]));
    }

    return array;
}

Napi::Object convertReelSetInfo(Napi::Env env, const std::map<std::string, long int>& data) {
    Napi::Object o = Napi::Object::New(env);

    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string name = it->first;
        long int count   = it->second;

        o.Set(name, count);
    }

    return o;
}

Napi::Object winMapToNapi (Napi::Env env, const std::map<double, long int>& winMap) {
    Napi::Object obj = Napi::Object::New(env);

    for (auto it = winMap.begin(); it != winMap.end(); ++it) {
        obj.Set(std::to_string(it->first), it->second);
    }

    return obj;
}

Napi::Object harnessOutputToNapiObject(Napi::Env env, const HarnessOutput& data) {

    // Convert batch info
    Napi::Object batchBaseSpin = Napi::Object::New(env);
    batchBaseSpin.Set("totalWins",     Napi::Number::New(env, data.batch.baseSpin.totalWins));
    batchBaseSpin.Set("rtp",           Napi::Number::New(env, data.batch.baseSpin.rtp));

    Napi::Object batch = Napi::Object::New(env);
    batch.Set("iterations",           Napi::Number::New(env, data.batch.iterations));
    batch.Set("totalWins",            Napi::Number::New(env, data.batch.totalWins));
    batch.Set("respinWins",           Napi::Number::New(env, data.batch.respinWins));
    batch.Set("baseCoinWins",         Napi::Number::New(env, data.batch.baseCoinWins));
    batch.Set("baseLineWins",         Napi::Number::New(env, data.batch.baseLineWins));
    batch.Set("freespinWins",         Napi::Number::New(env, data.batch.freespinWins));
    batch.Set("fsLineWins",           Napi::Number::New(env, data.batch.fsLineWins));
    batch.Set("fsCoinWins",           Napi::Number::New(env, data.batch.fsCoinWins));
    batch.Set("fsRespinWins",         Napi::Number::New(env, data.batch.fsRespinWins));
    batch.Set("totalBets",            Napi::Number::New(env, data.batch.totalBets));

    batch.Set("fsTotalCount",         Napi::Number::New(env, data.batch.fsTotalCount));
    batch.Set("respinCount",          Napi::Number::New(env, data.batch.respinCount));
    batch.Set("freespinCount",        Napi::Number::New(env, data.batch.freespinCount));
    batch.Set("respinWinCount",       Napi::Number::New(env, data.batch.respinWinCount));
    batch.Set("respinOccCount",       Napi::Number::New(env, data.batch.respinOccCount));
    batch.Set("freespinWinCount",     Napi::Number::New(env, data.batch.freespinWinCount));
    batch.Set("winCount",             Napi::Number::New(env, data.batch.winCount));
    batch.Set("sbCount",              Napi::Number::New(env, data.batch.sbCount));

    batch.Set("maxWin",               Napi::Number::New(env, data.batch.maxWin));
    batch.Set("maxWinCount",          Napi::Number::New(env, data.batch.maxWinCount));
    batch.Set("rtp",                  Napi::Number::New(env, data.batch.rtp));
    batch.Set("baseLineRTP",          Napi::Number::New(env, data.batch.baseLineRTP));
    batch.Set("baseCoinRTP",          Napi::Number::New(env, data.batch.baseCoinRTP));
    batch.Set("respinRTP",            Napi::Number::New(env, data.batch.respinRTP));
    batch.Set("freespinRTP",          Napi::Number::New(env, data.batch.freespinRTP));
    batch.Set("duration",             Napi::Number::New(env, data.batch.duration));
    batch.Set("reelSets",             convertReelSetInfo(env, data.batch.reelSetMap));
    batch.Set("winMap",               winMapToNapi(env, data.batch.winMap));
    batch.Set("fsCounts",             convertFsCounts(env, data.batch.fsCounts));
    batch.Set("baseSpin",             batchBaseSpin);

    // Convert total info
    Napi::Object totalBaseSpin = Napi::Object::New(env);
    totalBaseSpin.Set("totalWins",     Napi::Number::New(env, data.total.baseSpin.totalWins));
    totalBaseSpin.Set("rtp",           Napi::Number::New(env, data.total.baseSpin.rtp));

    Napi::Object total = Napi::Object::New(env);
    total.Set("iterations",            Napi::Number::New(env, data.total.iterations));
    total.Set("totalWins",             Napi::Number::New(env, data.total.totalWins));
    total.Set("baseCoinWins",          Napi::Number::New(env, data.total.baseCoinWins));
    total.Set("baseLineWins",          Napi::Number::New(env, data.total.baseLineWins));
    total.Set("respinWins",            Napi::Number::New(env, data.total.respinWins));
    total.Set("freespinWins",          Napi::Number::New(env, data.total.freespinWins));
    total.Set("totalBets",             Napi::Number::New(env, data.total.totalBets));

    total.Set("fsTotalCount",          Napi::Number::New(env, data.total.fsTotalCount));
    total.Set("freespinCount",         Napi::Number::New(env, data.total.freespinCount));
    total.Set("respinCount",           Napi::Number::New(env, data.total.respinCount));
    total.Set("respinOccCount",        Napi::Number::New(env, data.total.respinOccCount));
    total.Set("respinWinCount",        Napi::Number::New(env, data.total.respinWinCount));
    total.Set("freespinWinCount",      Napi::Number::New(env, data.total.freespinWinCount));

    total.Set("winCount",              Napi::Number::New(env, data.total.winCount));
    total.Set("sbCount",               Napi::Number::New(env, data.total.sbCount));
    total.Set("maxWin",                Napi::Number::New(env, data.total.maxWin));
    total.Set("maxWinCount",           Napi::Number::New(env, data.total.maxWinCount));
    total.Set("rtp",                   Napi::Number::New(env, data.total.rtp));
    total.Set("baseLineRTP",           Napi::Number::New(env, data.total.baseLineRTP));
    total.Set("baseCoinRTP",           Napi::Number::New(env, data.total.baseCoinRTP));
    total.Set("respinRTP",             Napi::Number::New(env, data.total.respinRTP));
    total.Set("freespinRTP",           Napi::Number::New(env, data.total.freespinRTP));
    total.Set("mean",                  Napi::Number::New(env, data.total.mean));
    total.Set("variance",              Napi::Number::New(env, data.total.variance));
    total.Set("reelSets",     convertReelSetInfo(env, data.total.reelSetMap));
    total.Set("fsCounts",     convertFsCounts(env, data.total.fsCounts));
    total.Set("baseSpin",     totalBaseSpin);

    Napi::Object obj = Napi::Object::New(env);

    obj.Set("batch", batch);
    obj.Set("total", total);

    return obj;
}
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
