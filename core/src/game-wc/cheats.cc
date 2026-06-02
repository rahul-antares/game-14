#include "../utils/log.h"
#include "../utils/error.h"
#include "constants.h"
#include "napi.h"
#include <array>
#include "round.h"
#include "../utils/error.h"
#include <cstdint>
#include <iostream>
#include "cheats.h"
#include "../utils/extract.h"

void getBool (const Napi::Object obj, const char* key, bool& ref) {
    Napi::Value val = obj.Get(key);
    if (!val.IsNull() && !val.IsUndefined() && val.IsBoolean()) {
        ref = val.As<Napi::Boolean>().Value();
    }
}

void getBoolAsInt (const Napi::Object obj, const char* key, int& ref) {
    Napi::Value val = obj.Get(key);
    if (!val.IsNull() && !val.IsUndefined() && val.IsBoolean()) {
        ref = val.As<Napi::Boolean>().Value() ? 1 : 0;
    }
}

void getInt (const Napi::Object obj, const char* key, int& ref) {
    Napi::Value val = obj.Get(key);
    if (!val.IsNull() && !val.IsUndefined() && val.IsNumber()) {
        ref = val.As<Napi::Number>().Int64Value();
    }
}

void getDouble (const Napi::Object obj, const char* key, double& ref) {
    Napi::Value val = obj.Get(key);
    if (!val.IsNull() && !val.IsUndefined() && val.IsNumber()) {
        ref = val.As<Napi::Number>().DoubleValue();
    }
}

void getString (const Napi::Object obj, const char* key, std::string& ref) {
    Napi::Value val = obj.Get(key);
    if (!val.IsNull() && !val.IsUndefined() && val.IsString()) {
        ref = val.As<Napi::String>().Utf8Value();
    }
}

void get2dVec (const Napi::Value& val, std::vector<std::vector<int>>& int2dVec) {
    if (val.IsNull() || val.IsUndefined() || !val.IsArray())
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        Napi::Value val     = objArr.Get(i);
        auto [valtype, str] = getValueType(val);

        std::vector<int> temp;
        getArray (val, temp);
        int2dVec.push_back(temp);
    }
}

void getStopsOfStops (const Napi::Value& val, std::vector<std::array<std::int32_t, REEL_COUNT>>& int2dArr) {
    if (val.IsNull() || val.IsUndefined() || !val.IsArray())
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        std::vector<std::int32_t> resArr;
        getArray(objArr.Get(i), resArr);

        std::array<int32_t, REEL_COUNT> stops;
        stops.fill(-1);

        for (auto j = 0; j < resArr.size(); j++) {
            stops[j] = resArr[j];
        }

        int2dArr.push_back(stops);
    }
}

void validateCheatImpl(const Napi::Env env, const Napi::Object cheatObj) {
    Napi::Array cheatkeys = cheatObj.GetPropertyNames();

    for (auto i = 0; i < cheatkeys.Length(); i++) {
        Napi::Value keyVal = cheatkeys[i];
        std::string key    = keyVal.As<Napi::String>().Utf8Value();

        bool exists = false;
        for (auto cheat: CheatsAll) {
            if (cheat.name == key) {
                exists = true;

                if (!cheat.implemented)
                    throw_error("Cheat [", key, "] NOT implemented yet!");

                break;
            }
        }

        if (!exists)
            throw_error("Cheat [", key, "] doesn't exist!");
    }
}

void validateSizeCheat (const std::vector<int>& v, const std::string msg) {
    if (v.size() != 2 && v.size() > 0) {
        throw_error ("size expects exactly 2 arguments, cheat::", msg);
    } else if (v.size() == 2) {
        int x = v[0];
        int y = v[1];
        if (x <= 0 || x > 5 || y <= 0 || y > 5)
            throw_error ("Invalid random-feature-size, x = ", x, ", y = ", y);
    }
}

void getStops (const Napi::Object& obj, Cheats& cheats) {
    Napi::Array stops = obj.Get ("stops").As<Napi::Array>();

    if (!stops.IsNull() && !stops.IsUndefined() && stops.IsArray()) {
        for (uint32_t i = 0; i < stops.Length(); i++) {
            Napi::Value item = stops[i];
            cheats.stops[i] = item.As<Napi::Number>().Int64Value();
        }
    }
}

void validateFeatureStr (const std::string& featStr) {
    const auto feat = FS_FEATURE_FROM_STR (featStr);
    if (feat == FsFeature::NONE)
        throw_error ("invalid fs-feature in cheats, ", featStr);
}

void getFeatures (const Napi::Object& obj, Cheats& cheats) {
    Napi::Array featuresArr = obj.Get ("features").As<Napi::Array>();

    if (!featuresArr.IsNull() && !featuresArr.IsUndefined() && featuresArr.IsArray()) {
        for (auto i = 0; i < featuresArr.Length(); i++) {
            std::string fstr = featuresArr.Get(i).As<Napi::String>();
            validateFeatureStr (fstr);
            cheats.features.push_back(fstr);
        }
    }
}

Cheats parseCheats(const Napi::Env env, const Napi::Value c) {

    Cheats cheats;
    cheats.stops.fill(-1);

    // Else cheats is also provided as the 2nd option
    if (!c.IsObject())
        throw Napi::Error::New(env, "invalid option type");

    Napi::Object obj = c.As<Napi::Object>().ToObject();
    validateCheatImpl (env, obj);

    extract (obj, "minWin",         cheats.minWin, false, true, 0, MAX_WIN_CAP);
    extract (obj, "maxWin",         cheats.maxWin, false, true, 0, MAX_WIN_CAP);
    extract (obj, "winSymbol",      cheats.winSymbol, false);
    extract (obj, "winSymbolCount", cheats.winSymbolCount, false, true, 0, 5);
    extract (obj, "excludeSymbol",  cheats.excludeSymbol, false);
    extract (obj, "paylineWin",     cheats.paylineWin, false, true, 0, NUM_PAYLINES-1);

    getStops (obj, cheats);
    getFeatures(obj, cheats);

    extract (obj, "reelset",     cheats.reelset);
    extract (obj, "coinValue",   cheats.coinValue);
    extract (obj, "numCoins",    cheats.numCoins);
    extract (obj, "numScatters", cheats.numScatters);
    extract (obj, "minCollectionLevel", cheats.minCollectionLevel, false, true, 1, 4);
    extract (obj, "maxCollectionLevel", cheats.maxCollectionLevel, false, true, 1, 4);
    extract (obj, "numFeatures", cheats.numFeatures, false, true, 0, 5);

    getBoolAsInt (obj, "respin",       cheats.respin);
    getBoolAsInt (obj, "scatterHook",  cheats.scatterHook);
    getBoolAsInt (obj, "freespins",    cheats.freespins);
    getBoolAsInt (obj, "winCap",       cheats.winCap);
    getBoolAsInt (obj, "topRowRespin", cheats.topRowRespin);
    getBoolAsInt (obj, "fsDoubleWild", cheats.fsDoubleWild);

    extract (obj, "fsCoinValue",  cheats.fsCoinValue, false, true, 0, 10000);
    extract (obj, "fsCoinValueCollected",  cheats.fsCoinValueCollected, false, true, 0, 10000);
    extract (obj, "fsCoinValueEveryLevel",  cheats.fsCoinValueEveryLevel, false, true, 0, 10000);
    extract (obj, "fsCoinValueCollectedEveryLevel",  cheats.fsCoinValueCollectedEveryLevel, false, true, 0, 10000);
    extract (obj, "minFSWin", cheats.minFSWin, false, true, 0, MAX_WIN_CAP);
    extract (obj, "maxFSWin", cheats.maxFSWin, false, true, 0, MAX_WIN_CAP);
    extract ( obj, "gambleSuccessLevel", cheats.gambleSuccessLevel, false, true, 1, 5);

    getStopsOfStops ( obj.Get("fsStops"), cheats.fsStops);

    LOG("minWin             = %f\n", cheats.minWin);
    LOG("maxWin             = %f\n", cheats.maxWin);
    LOG("winSymbol          = %s\n", cheats.winSymbol.data());
    LOG("winSymbolCount     = %d\n", cheats.winSymbolCount);
    LOG("excludeSymbol      = %s\n", cheats.excludeSymbol.data());

    LOG("stops              = ");
    std::for_each(
        cheats.stops.begin(),
        cheats.stops.end(),
        []([[ maybe_unused ]] const int n) { LOG("%d,\n", n); }
    );
    LOG("\n");

    return cheats;
}
