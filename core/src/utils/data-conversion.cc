#include <string>

#include "bets.h"
#include "constants.h"
#include "napi.h"
#include "rng.h"
#include "../utils/log.h"
#include "../utils/extract.h"
#include "../game/data-defs.h"
#include "round.h"
#include "symbols.h"
#include "./data-conversion.h"

Napi::Object fillReelSets(Napi::Env& env, std::string prefix, const SetOfReelSets* set) {
    Napi::Object obj = Napi::Object::New(env);

    for (unsigned int setIdx = 0; setIdx < set->sets.size(); setIdx++) {
        Set* currSet      = set->sets[setIdx];
        Napi::Array reelObj    = Napi::Array::New(env, currSet->getReels().size());

        for (unsigned int i = 0; i < currSet->getReels().size(); i++) {
            Reel* reel = currSet->getReels()[i];
            reelObj.Set(i, reelToNapiObject(env, reel->getReel()));
        }

        obj.Set(currSet->name.c_str(), reelObj);
    }

    return obj;
}

// Helper function to create static game engine config
Napi::Object createConfigObject(Napi::Env &env, GameConfig& config) {
    Napi::Object configObject = Napi::Object::New(env);

    configObject.Set("creditsPerBet",   intToNapi(env, CREDITS_PER_BET));
    configObject.Set("maxWinCap",       doubleToNapi(env, MAX_WIN_CAP));
    configObject.Set("gameCore",        stringToNapi(env, config.gameCore));
    configObject.Set("gameVersion",     stringToNapi(env, config.gameVersion));
    configObject.Set("gameName",        stringToNapi(env, config.gameName));
    configObject.Set("studio",          stringToNapi(env, config.gameStudio));
    configObject.Set("variant",         stringToNapi(env, config.variant));
    configObject.Set("parsheetVersion", stringToNapi(env, config.parsheetVersion));
    configObject.Set("probFreeSpinsBonusBoost",   doubleToNapi(env, config.probFreeSpinsBB));
    configObject.Set("probFreeSpinsSuperBoost",   doubleToNapi(env, config.probFreeSpinsSB));

    // Set paylines array
    Napi::Array paylines = Napi::Array::New(env, config.paylines.size());
    for (unsigned int i = 0; i < config.paylines.size(); i++) {
        Napi::Array line = Napi::Array::New(env, config.paylines[i].size());

        for (unsigned int j = 0; j < config.paylines[i].size(); j++) {
            line.Set(j, intToNapi(env, config.paylines[i][j]));
        }

        paylines.Set(i, line);
    }

    configObject.Set("paylines", paylines);


    // setting the ladder weights
    Napi::Array lw = Napi::Array::New(env, config.ladderWeights.size() - 1);
    for (int i = 0; i < config.ladderWeights.size(); i++) {
        Napi::Object lwx = Napi::Object::New(env);
        lwx.Set("successProb", doubleToNapi (env, config.ladderWeights[i]));
        lwx.Set("level", intToNapi (env, i+1));

        lw.Set(i, lwx);
    }
    configObject.Set("gamble", lw);

    // Set paytable array
    Napi::Array paytable = Napi::Array::New(env, config.payTable.size());
    for (unsigned int i = 0; i < config.payTable.size(); i++) {
        PayTbl tblItem     = config.payTable[i];
        Napi::Object entry = Napi::Object::New(env);

        entry.Set("symbol",   symbolToNapi(env, tblItem.symbol));
        entry.Set("pay",      intToNapi(env, tblItem.pay));
        entry.Set("count",    intToNapi(env, tblItem.count));
        paytable.Set(i, entry);
    }
    configObject.Set("paytable", paytable);

    // Set bet configs
    Napi::Object betConfigs = Napi::Object::New(env);
    for (unsigned int i = 0; i < config.betConfig.size(); i++) {
        Napi::Object betObj = Napi::Object::New(env);
        BetConfig item      = config.betConfig[i];

        betObj.Set("cost",       intToNapi(env, item.cost));
        betObj.Set("defaultMax", doubleToNapi(env, item.defaultMax));
        betObj.Set("rtp",        doubleToNapi(env, item.rtp));
        betObj.Set("mode",       intToNapi(env, item.mode));
        betObj.Set("type",       stringToNapi(env, item.type));
        betObj.Set("maxWin",     doubleToNapi(env, item.maxWin));

        Napi::Array betSizes = Napi::Array::New(env, item.size);
        for (unsigned int k = 0; k < item.size; k++) {
            betSizes.Set(k, intToNapi(env, item.allowed[k]));
        }

        betObj.Set("bets", betSizes);
        betConfigs.Set(item.name, betObj);
    }

    for (auto i = 0; i < config.esbetConfig.size(); i++) {
        Napi::Object betObj = Napi::Object::New(env);
        const auto item      = config.esbetConfig[i];

        betObj.Set("cost",       doubleToNapi (env, item.cost));
        betObj.Set("defaultMax", doubleToNapi (env, item.defaultMax));
        betObj.Set("rtp",        doubleToNapi (env, item.rtp));
        betObj.Set("mode",       intToNapi (env, item.mode));
        betObj.Set("type",       stringToNapi (env, item.type));
        betObj.Set("maxWin",     doubleToNapi (env, item.maxWin));

        Napi::Array betSizes = Napi::Array::New(env, item.size);
        for (unsigned int k = 0; k < item.size; k++) {
            betSizes.Set (k, doubleToNapi (env, item.allowed[k]));
        }

        betObj.Set("bets", betSizes);
        betConfigs.Set(item.name, betObj);

    }

    configObject.Set("betConfigs", betConfigs);

    // setting the ladder weights
    // configObject.Set("gamble", lw);

    // Set reelSets object
    Napi::Object reelSets   = Napi::Object::New(env);
    Napi::Object RGreelSets = fillReelSets(env, "regularReelSet",    config.rgsets);
    Napi::Object BBreelSets = fillReelSets(env, "bonusBoostReelSet", config.bbsets);
    Napi::Object SBreelSets = fillReelSets(env, "superBoostReelSet", config.sbsets);
    Napi::Object FSreelSets = fillReelSets(env, "fsReelset",         config.fssets);

    reelSets.Set("regular",       RGreelSets);
    reelSets.Set("bonusBoost",    BBreelSets);
    reelSets.Set("superBoost",    SBreelSets);
    reelSets.Set("freespin",      FSreelSets);

    configObject.Set("reelsets", reelSets);
    return configObject;
}

// Helper function to convert Reel to Napi::Object
Napi::Array reelToNapiObject(Napi::Env &env, const std::vector<Symbol> reelSymbols) {
    Napi::Array symbols = Napi::Array::New(env, reelSymbols.size());
    for (unsigned int i = 0; i < reelSymbols.size(); ++i) {
        symbols.Set(uint32_t(i), SymbolChar[reelSymbols[i]]);
    }
    return symbols;
}

// Helper function to convert Coordinate to Napi::Object
Napi::Object coordinateToNapiObject(Napi::Env env, const Coordinate& coord) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("x", intToNapi (env, coord.x));
    obj.Set("y", intToNapi (env, coord.y));
    return obj;
}

// Helper function to convert Winning to Napi::Object
Napi::Object winningToNapiObject(Napi::Env env, const Winning& win) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("type",         stringToNapi(env, win.type));

    obj.Set("winAmount",    intToNapi (env, win.winAmount));
    obj.Set("refWinAmount", intToNapi (env, win.refWinAmount));

    obj.Set("payLine",      intToNapi (env, win.payLine));
    obj.Set("symbol",       stringToNapi(env, SymbolChar[win.symbol]));
    obj.Set("dir",          stringToNapi(env, win.dir));
    obj.Set("symCount",     intToNapi (env, win.symCount));
    obj.Set("multiplier",   intToNapi (env, win.multiplier));

    Napi::Array coordinatesArray = Napi::Array::New(env, win.coords.size());
    for (size_t i = 0; i < win.coords.size(); ++i) {
        coordinatesArray.Set(i, coordinateToNapiObject(env, win.coords[i]));
    }
    obj.Set("coords", coordinatesArray);

    return obj;
}

Napi::Object symCoordsTONAPI(Napi::Env env, const SymCoords& symCoords) {
    Napi::Object o = coordinateToNapiObject (env, symCoords);
    o.Set("symbol", Napi::String::New(env, SymbolChar[symCoords.symbol]));
    return o;
}

Napi::Object windowToNapi (Napi::Env env, const Window& window) {
    if (!window.filled)
        return Napi::Array::New(env, 0);

    const auto& grid     = window.grid;
    Napi::Array gridNAPI = Napi::Array::New(env, grid.size());

    for (size_t i = 0; i < grid.size(); ++i) {
        Napi::Array o = Napi::Array::New(env, grid[i].size());

        for (size_t j = 0; j < grid[i].size(); ++j) {
            o.Set(j, Napi::String::New(env, SymbolChar[grid[i][j]]));
        }

        gridNAPI.Set(i, o);
    }

    return gridNAPI;
}

Napi::Object gridToNAPI(Napi::Env env, const Grid& grid, bool empty) {
    if (empty)
        return Napi::Array::New(env, 0);

    Napi::Array gridNAPI = Napi::Array::New(env, grid.size());

    for (size_t i = 0; i < grid.size(); ++i) {
        Napi::Array o = Napi::Array::New(env, grid[i].size());

        for (size_t j = 0; j < grid[i].size(); ++j) {
            o.Set(j, Napi::String::New(env, SymbolChar[grid[i][j]]));
        }

        gridNAPI.Set(i, o);
    }

    return gridNAPI;
}

// TODO: native conversion is not implemented yet
Napi::Object lockedSymbol_Vec_ToNAPI(Napi::Env env, const LockedSym_Vec& lockedSymbols) {
    Napi::Array lockedSymsArr = Napi::Array::New(env, lockedSymbols.size());

    for(int i=0; i<lockedSymbols.size(); i++) {
        Napi::Object o = Napi::Object::New(env);
        o.Set("source",      coordinateToNapiObject (env, lockedSymbols[i].source));
        o.Set("destination", coordinateToNapiObject (env, lockedSymbols[i].destination));
        o.Set("symbol",      Napi::String::New(env, SymbolChar[lockedSymbols[i].symbol]));

        lockedSymsArr.Set(i, o);
    }

    return lockedSymsArr;
}

Napi::Object winningsToNapiObject(Napi::Env env, const Winning_Vec& wins) {
    Napi::Array arr = Napi::Array::New(env);

    for (size_t i = 0; i < wins.size(); ++i) {
        arr.Set(i, winningToNapiObject(env, wins[i]));
    }

    return arr;
}

Napi::Object symCoordsToNAPI(Napi::Env env, const SymCoords& symCoords) {
    Napi::Object o = Napi::Object::New(env);
    o.Set("x",      intToNapi (env, symCoords.x));
    o.Set("y",      intToNapi (env, symCoords.y));
    o.Set("symbol", stringToNapi(env, SymbolChar[symCoords.symbol]));
    return o;
}

Napi::Object coinSymToNAPI(Napi::Env env, const CoinSymbol& symCoords) {
    Napi::Object o = Napi::Object::New(env);

    o.Set("x",        intToNapi (env, symCoords.x));
    o.Set("y",        intToNapi (env, symCoords.y));
    o.Set("symbol",   stringToNapi(env, SymbolChar[symCoords.symbol]));

    o.Set("value",    intToNapi (env, symCoords.value));
    o.Set("valueX",   intToNapi (env, symCoords.valueX));
    o.Set("refValue", intToNapi (env, symCoords.refValue));

    return o;
}

Napi::Object coinSym_VecToNAPI(Napi::Env env, const CoinSym_Vec& symCoordsV) {
    Napi::Array o = Napi::Array::New(env);

    for (size_t i = 0; i < symCoordsV.size(); i++)
        o.Set(i, coinSymToNAPI(env, symCoordsV[i]));

    return o;
}

Napi::Object symCoordsVecToNAPI(Napi::Env env, const SymCoords_Vec& symCoordsV) {
    Napi::Array o = Napi::Array::New(env);

    for (size_t i = 0; i < symCoordsV.size(); i++)
        o.Set(i, symCoordsToNAPI(env, symCoordsV[i]));

    return o;
}

Napi::Array stopsToNAPI(Napi::Env env, const std::array<int, REEL_COUNT>& stops, bool empty) {
    if (empty)
        return Napi::Array::New(env, 0);

    Napi::Array arr = Napi::Array::New(env);

    for (size_t i = 0; i < stops.size(); ++i) {
        arr.Set(i, Napi::Number::New(env, stops[i]));
    }

    return arr;
}

// Helper function to convert Respin to Napi::Object
Napi::Object spinToNAPI(Napi::Env env, const Spin& inputSpin, const bool empty) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set("index",             intToNapi (env, inputSpin.index));
    obj.Set("winAmount",         intToNapi (env, inputSpin.winAmount));
    obj.Set("refWinAmount",      intToNapi (env, inputSpin.refWinAmount));
    obj.Set("winsSoFar",         intToNapi (env, inputSpin.winsSoFar));
    obj.Set("refWinsSoFar",      intToNapi (env, inputSpin.refWinsSoFar));
    obj.Set("baseWinAmount",     intToNapi (env, inputSpin.baseWinAmount));
    obj.Set("refBaseWinAmount",  intToNapi (env, inputSpin.refBaseWinAmount));
    obj.Set("coinSymbolWins",    intToNapi (env, inputSpin.coinSymbolWins));
    obj.Set("refCoinSymbolWins", intToNapi (env, inputSpin.refCoinSymbolWins));
    obj.Set("maxWinTriggered",   boolToNapi (env, inputSpin.maxWinTriggered));

    obj.Set("scatterHook",   boolToNapi (env, inputSpin.scatterHook));
    obj.Set("stops",         stopsToNAPI(env, inputSpin.stops, empty));
    obj.Set("scatterHookStops", stopsToNAPI(env, inputSpin.scatterHookStops, empty));
    obj.Set("symbols",       gridToNAPI(env, inputSpin.symbols, empty));
    obj.Set("scatterHookSymbols", gridToNAPI(env, inputSpin.scatterHookSymbols, empty));
    obj.Set("lockedSymbols", lockedSymbol_Vec_ToNAPI(env, inputSpin.lockedSymbols));
    obj.Set("bonusSymbols",  symCoordsVecToNAPI(env, inputSpin.bonusSyms));
    obj.Set("wildSymbols",   symCoordsVecToNAPI(env, inputSpin.wildSyms));
    obj.Set("coinSymbols",   coinSym_VecToNAPI(env, inputSpin.coinSyms));

    obj.Set("type",              stringToNapi(env, inputSpin.type));
    obj.Set("coinValueSet",      stringToNapi(env, inputSpin.coinValueSet));
    obj.Set("numCR",             intToNapi (env, inputSpin.numCR));
    obj.Set("wsForcedPlacement", boolToNapi (env, inputSpin.wsForcedPlacement));
    obj.Set("newWsPlaced",       symCoordsVecToNAPI(env, inputSpin.newWsPlaced));
    obj.Set("rSymReplacements",  symCoordsVecToNAPI(env, inputSpin.rSymReplacements));
    obj.Set("overlayWsAndCrPlacements", symCoordsVecToNAPI(env, inputSpin.overlayWsAndCrPlacements));

    obj.Set("winnings",         winningsToNapiObject(env, inputSpin.winnings));
    obj.Set("reelSet",          stringToNapi(env, inputSpin.reelSet));
    // obj.Set("wsCollected",      intToNapi (env, inputSpin.wsCollected));
    obj.Set("wsCollectedSoFar", intToNapi (env, inputSpin.wsCollectedSoFar));
    obj.Set("newWsLanded",      intToNapi (env, inputSpin.newWsLanded));
    obj.Set("level",            intToNapi (env, inputSpin.level));
    obj.Set("coinMultiplier",   intToNapi (env, inputSpin.coinMultiplier));
    obj.Set("stoppedGrid",         gridToNAPI(env, inputSpin.stoppedGrid, empty));

    obj.Set("numSC", intToNapi (env, inputSpin.numSC));
    obj.Set("numWS", intToNapi (env, inputSpin.numWS));
    obj.Set("numCS", intToNapi (env, inputSpin.numCS));

#ifdef RAW_SYMMBOLS_INCL
    obj.Set("rawSymbols",   gridToNAPI(env, inputSpin.rawSymbols, empty));
#endif
    return obj;
}

Napi::Object baseSpinToNAPI(Napi::Env env, const BaseSpin& baseSpin, const bool empty) {
    Napi::Object obj =  spinToNAPI(env, baseSpin, empty);
    obj.Set("spinType", stringToNapi(env, baseSpin.spinType));

    Napi::Array reSpins = Napi::Array::New(env, baseSpin.reSpins.size());
    for (size_t i = 0; i < baseSpin.reSpins.size(); ++i) {
        Napi::Object o = reSpinToNAPI (env, baseSpin.reSpins[i], empty);
        reSpins.Set(i, o);
    }

    obj.Set("reSpins", reSpins);

    return obj;
}

Napi::Object freeSpinToNAPI(Napi::Env env, const FreeSpin& freeSpin, const bool empty) {
    Napi::Object obj = spinToNAPI(env, freeSpin, empty);

    obj.Set("runningWinAmount", Napi::Number::New(env, freeSpin.runningWinAmount));
    obj.Set("extraSpins",       Napi::Number::New(env, freeSpin.extraSpins));
    obj.Set("spinType",         Napi::String::New(env, freeSpin.spinType));

    Napi::Array reSpins = Napi::Array::New(env, freeSpin.reSpins.size());
    for (size_t i = 0; i < freeSpin.reSpins.size(); ++i) {
        Napi::Object o = reSpinToNAPI (env, freeSpin.reSpins[i], empty);
        reSpins.Set(i, o);
    }

    obj.Set("reSpins", reSpins);
    return obj;
}

Napi::Object reSpinToNAPI(Napi::Env env, const Spin& reSpin, const bool empty) {
    Napi::Object obj = spinToNAPI(env, reSpin, empty);
    return obj;
}

const char* StatusStr[] = {
    "INIT",
    "SUCCEEDED",
    "FAILED",
    "COLLECTED",
};

Napi::Object fsStatusToNapi(Napi::Env env, const std::vector<FsStatus>& fsStatus) {
    Napi::Object statusArr = Napi::Array::New(env);

    for (size_t i = 0; i < fsStatus.size(); i++) {
        const FsStatus& status = fsStatus.at(i);
        Napi::Object statusObj = Napi::Object::New(env);

        statusObj.Set("level",   intToNapi (env, status.level));
        statusObj.Set("draw",    doubleToNapi (env, status.draw));
        statusObj.Set("status",  stringToNapi(env, StatusStr[status.status]));

        statusArr.Set(i, statusObj);
    }

    return statusArr;
}

Napi::Object internalToNapi(Napi::Env env, const Internal& internal) {
    const std::vector<RandRecord>& history = internal.rHistory;

    Napi::Object o = Napi::Object::New(env);
    Napi::Array r  = Napi::Array::New(env);

    o.Set("rand", r);

    for (size_t i = 0; i < history.size(); i++) {
        const RandRecord& hItem = history[i];
        Napi::Object record = Napi::Object::New(env);

        record.Set("bStream", Napi::Buffer<uint64_t>::Copy(env, &hItem.bStream, 1));
        record.Set("scaled",  Napi::Number::New(env, hItem.scaled));
        record.Set("max",     Napi::Number::New(env, hItem.max));

        r.Set(i, record);
    }

    return o;
}

Napi::Object buyExtraSpinToNAPI (Napi::Env env, const BuyExtraSpin& bes) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set ("extraSpin",    boolToNapi (env, bes.extraSpin));
    obj.Set ("expectedBet",  doubleToNapi (env, bes.expectedBet));
    obj.Set ("costX",        doubleToNapi (env, bes.costX));
    obj.Set ("featureMode",  intToNapi (env, bes.featureMode));
    obj.Set ("id",           intToNapi (env, bes.id));

    return obj;
}

// Function to convert GameData to Napi::Object
Napi::Object playResponseToNAPI(Napi::Env env, const PlayResponse& data) {
    Napi::Object obj = Napi::Object::New(env);

    obj.Set ("betAmount",          intToNapi (env, data.betAmount));
    obj.Set ("currencyMultiplier", intToNapi (env, data.currencyMultiplier));
    obj.Set ("refBetAmount",       intToNapi (env, data.refBetAmount));
    obj.Set ("refBetBase",         intToNapi (env, data.refBetBase));
    obj.Set ("ended",              boolToNapi (env, data.ended));
    obj.Set ("extraSpin",          boolToNapi (env, data.extraSpin));
    obj.Set ("persistent",         boolToNapi (env, data.persistent));
    obj.Set ("empty",              boolToNapi (env, data.empty));
    obj.Set ("action",             stringToNapi(env, data.action));
    obj.Set ("featureMode",        intToNapi (env, data.featureMode));
    obj.Set ("subgameTriggered",   boolToNapi (env, data.subgameTriggered));
    obj.Set ("winAmount",          intToNapi (env, data.winAmount));
    obj.Set ("refWinAmount",       intToNapi (env, data.refWinAmount));
    obj.Set ("maxWinTriggered",    boolToNapi (env, data.maxWinTriggered));
    obj.Set ("fsStatus",           fsStatusToNapi(env, data.fsStatus));
    obj.Set ("buyExtraSpin", buyExtraSpinToNAPI(env, data.buyExtraSpin));

    // convert baseSpin
    bool emptySpin = (data.featureMode >= MODE_FEATURE_BUY1 ? true : data.empty);
    Napi::Object baseSpin = baseSpinToNAPI(env, data.baseSpin, emptySpin);
    obj.Set("baseSpin", baseSpin);

    // convert freeSpins
    Napi::Array freeSpins = Napi::Array::New(env, data.freeSpins.size());
    for (size_t i = 0; i < data.freeSpins.size(); ++i) {
        Napi::Object o = freeSpinToNAPI(env, data.freeSpins[i], data.empty);
        freeSpins.Set(i, o);
    }
    obj.Set("freeSpins", freeSpins);

    // active features
    Napi::Array activeFeatures = Napi::Array::New(env);
    for (size_t i = 0; i < data.activeFeatures.size(); i++)
        activeFeatures.Set (i, FS_FEATURE_STR (data.activeFeatures[i]));

    obj.Set ("activeFeatures", activeFeatures);

#ifdef RAW_SYMMBOLS_INCL
    // Convert rawSymbols array
    Napi::Array rawSymbolsArray = Napi::Array::New(env, data.rawSymbols.size());
    for (size_t i = 0; i < data.rawSymbols.size(); ++i) {
        Napi::Array symbolRow = Napi::Array::New(env, data.rawSymbols[i].size());
        for (size_t j = 0; j < data.rawSymbols[i].size(); ++j) {
            symbolRow.Set(j, Napi::String::New(env, SymbolChar[data.rawSymbols[i][j]]));
        }
        Napi::Object reelSymbols = Napi::Object::New(env);
        reelSymbols.Set("reel", symbolRow);
        rawSymbolsArray.Set(i, reelSymbols);
    }
    obj.Set("rawSymbols", rawSymbolsArray);
#endif

    obj.Set("internal", internalToNapi(env, data.internal));
    return obj;
}

std::vector<FsStatus> toFsStatusNative(Napi::Array fsArray) {
    std::vector<FsStatus> fs;

    for (size_t i = 0; i < fsArray.Length(); i++) {
        FsStatus f;

        Napi::Object obj = fsArray.Get(i).As<Napi::Object>();
        f.level          = obj.Get("level").As<Napi::Number>().Int64Value();
        f.draw           = obj.Get("draw").As<Napi::Number>().DoubleValue();
        f.status         = FS_STATUS_MAP.at(obj.Get("status").As<Napi::String>());

        fs.push_back(f);
    }

    return fs;
}

SymCoords_Vec toSymCoords_Vec_Native(Napi::Array o) {
    SymCoords_Vec v;

    for (size_t i = 0; i < o.Length(); i++) {
        Napi::Object el = o.Get(i).As<Napi::Object>();
        std::string symbolStr   = el.Get("symbol").As<Napi::String>().Utf8Value();
        // if (symbolStr.size() == 0 || symbolStr.empty())
        //     continue;
        Symbol symbol   = SymMap.at(symbolStr);
        int x           = el.Get("x").As<Napi::Number>().Int64Value();
        int y           = el.Get("y").As<Napi::Number>().Int64Value();

        SymCoords c(x, y, symbol);
        v.push_back(c);
    }

    return v;
}

CoinSym_Vec toCoinSym_Vec_Native(Napi::Array o) {
    CoinSym_Vec v;

    for (size_t i = 0; i < o.Length(); i++) {
        Napi::Object el = o.Get(i).As<Napi::Object>();
        Symbol symbol   = SymMap.at(el.Get("symbol").As<Napi::String>());
        int x           = el.Get("x").As<Napi::Number>().Int64Value();
        int y           = el.Get("y").As<Napi::Number>().Int64Value();
        int value       = el.Get("value").As<Napi::Number>().Int64Value();
        int valueX      = el.Get("valueX").As<Napi::Number>().Int64Value();
        int refValue    = el.Get("refValue").As<Napi::Number>().Int64Value();

        CoinSymbol c{x, y, symbol, valueX, value, refValue};
        v.push_back(c);
    }

    return v;
}

// NOTE: very dangerous code part
Grid toGridNative(Napi::Array o) {
    Grid grid;

    for (size_t i = 0; i < o.Length(); i++) {
        Napi::Array reel = o.Get(i).As<Napi::Array>();

        for (size_t j = 0; j < reel.Length(); j++) {
            grid[i][j] = SymMap.at (reel.Get(j).As<Napi::String>());
        }
    }

    return grid;
}

Window toWindowNative (Napi::Array o) {
    Window w;

    for (size_t i = 0; i < o.Length(); i++) {
        Napi::Array reel = o.Get(i).As<Napi::Array>();

        for (size_t j = 0; j < reel.Length(); j++) {
            w.grid[i][j] = SymMap.at (reel.Get(j).As<Napi::String>());
        }
    }

    return w;
}

std::vector<Coordinate> toCoords_Vec(Napi::Array o) {
    std::vector<Coordinate> coords;

    for (size_t i = 0; i < o.Length(); i++) {
        Napi::Object el = o.Get(i).As<Napi::Object>();

        int x = el.Get("x").As<Napi::Number>().Int64Value();
        int y = el.Get("y").As<Napi::Number>().Int64Value();
        Coordinate c(x,y);

        coords.push_back(c);
    }

    return coords;
}

Winning_Vec toWinnings_Vec_Native(Napi::Array o) {
    Winning_Vec v;

    for (size_t i = 0; i < o.Length(); i++) {
        Napi::Object el = o.Get(i).As<Napi::Object>();
        Winning win;

        win.type         = el.Get("type").As<Napi::String>();
        win.winAmount    = el.Get("winAmount").As<Napi::Number>().Int64Value();
        win.refWinAmount = el.Get("refWinAmount").As<Napi::Number>().Int64Value();
        win.payLine      = el.Get("payLine").As<Napi::Number>().Int64Value();
        win.symCount     = el.Get("symCount").As<Napi::Number>().Int64Value();
        win.multiplier   = el.Get("multiplier").As<Napi::Number>().Int64Value();
        win.dir          = el.Get("dir").As<Napi::String>();
        win.symbol       = SymMap.at(el.Get("symbol").As<Napi::String>());
        win.coords       = toCoords_Vec(el.Get("coords").As<Napi::Array>());

        v.push_back(win);
    }

    return v;
}

std::array<int, REEL_COUNT> toInt_Vec_Native(Napi::Array o) {
    std::array<int, REEL_COUNT> v;

    for (size_t i = 0; i < o.Length(); i++) {
        int stop = o.Get(i).As<Napi::Number>().Int64Value();
        v[i] = stop;
    }

    return v;
}

void toSpinNative(Napi::Object o, Spin& s) {
    s.index               = o.Get("index").As<Napi::Number>().Int32Value();
    s.numWS               = o.Get("numWS").As<Napi::Number>().Int64Value();
    s.numSC               = o.Get("numSC").As<Napi::Number>().Int64Value();
    s.numCS               = o.Get("numCS").As<Napi::Number>().Int64Value();
    // s.wsCollected         = o.Get("wsCollected").As<Napi::Number>().Int64Value();
    s.wsCollectedSoFar    = o.Get("wsCollectedSoFar").As<Napi::Number>().Int64Value();
    s.newWsLanded    = o.Get("newWsLanded").As<Napi::Number>().Int64Value();
    s.coinMultiplier      = o.Get("coinMultiplier").As<Napi::Number>().Int64Value();
    s.level               = o.Get("level").As<Napi::Number>().Int64Value();

    s.winAmount           = o.Get("winAmount").As<Napi::Number>().Int64Value();
    s.winsSoFar           = o.Get("winsSoFar").As<Napi::Number>().Int64Value();
    s.baseWinAmount       = o.Get("baseWinAmount").As<Napi::Number>().Int64Value();
    s.coinSymbolWins      = o.Get("coinSymbolWins").As<Napi::Number>().Int64Value();
    s.refWinAmount        = o.Get("refWinAmount").As<Napi::Number>().Int64Value();
    s.refWinsSoFar        = o.Get("refWinsSoFar").As<Napi::Number>().Int64Value();
    s.refBaseWinAmount    = o.Get("refBaseWinAmount").As<Napi::Number>().Int64Value();
    s.refCoinSymbolWins   = o.Get("refCoinSymbolWins").As<Napi::Number>().Int64Value();

    s.type                     = o.Get("type").As<Napi::String>();
    s.coinValueSet             = o.Get("coinValueSet").As<Napi::String>();
    s.numCR                    = o.Get("numCR").As<Napi::Number>().Int64Value();
    s.wsForcedPlacement        = o.Get("wsForcedPlacement").As<Napi::Boolean>();
    s.newWsPlaced              = toSymCoords_Vec_Native(o.Get("newWsPlaced").As<Napi::Array>());
    s.rSymReplacements         = toSymCoords_Vec_Native(o.Get("rSymReplacements").As<Napi::Array>());
    s.overlayWsAndCrPlacements = toSymCoords_Vec_Native(o.Get("overlayWsAndCrPlacements").As<Napi::Array>());

    s.reelSet             = o.Get("reelSet").As<Napi::String>();
    s.maxWinTriggered     = o.Get("maxWinTriggered").As<Napi::Boolean>();
    s.scatterHook         = o.Get("scatterHook").As<Napi::Boolean>();
    s.winnings            = toWinnings_Vec_Native(o.Get("winnings").As<Napi::Array>());
    s.stops               = toInt_Vec_Native(o.Get("stops").As<Napi::Array>());
    s.scatterHookStops    = toInt_Vec_Native(o.Get("scatterHookStops").As<Napi::Array>());
    s.symbols             = toGridNative(o.Get("symbols").As<Napi::Array>());
    s.scatterHookSymbols  = toGridNative(o.Get("scatterHookSymbols").As<Napi::Array>());

    s.stoppedGrid         = toGridNative(o.Get("stoppedGrid").As<Napi::Array>());
    s.bonusSyms           = toSymCoords_Vec_Native(o.Get("bonusSymbols").As<Napi::Array>());
    s.wildSyms            = toSymCoords_Vec_Native(o.Get("wildSymbols").As<Napi::Array>());
    s.coinSyms            = toCoinSym_Vec_Native(o.Get("coinSymbols").As<Napi::Array>());

#ifdef RAW_SYMMBOLS_INCL
    s.rawSymbols = toGridNative(o.Get("rawSymbols").As<Napi::Array>());
#endif
}

std::vector<Spin> toRespin_Vec_Native(Napi::Array o) {
    std::vector<Spin> reSpins;

    for (size_t i = 0; i < o.Length(); i++) {
        Napi::Object el = o.Get(i).As<Napi::Object>();
        ReSpin reSpin;
        toSpinNative (el, reSpin);
        reSpins.push_back(reSpin);
    }

    return reSpins;
}

BaseSpin toBaseSpinNative(Napi::Object o) {
    BaseSpin b;
    toSpinNative(o, b);
    b.reSpins = toRespin_Vec_Native(o.Get("reSpins").As<Napi::Array>());
    return b;
}

void toRHistoryNative(Napi::Array& rand, std::vector<RandRecord>& rHistory) {
    for (size_t i = 0; i < rand.Length(); i++) {
        RandRecord r;
        Napi::Object o = rand.Get(i).As<Napi::Object>();

        r.bStream = o.Get("bStream").As<Napi::Number>().Int64Value();
        r.scaled  = o.Get("scaled").As<Napi::Number>().Int64Value();
        r.max     = o.Get("max").As<Napi::Number>().Int64Value();

        rHistory.push_back(r);
    }
}

BuyExtraSpin buyExtraSpinToNative (Napi::Object besObj) {
    BuyExtraSpin bes;

    extract (besObj, "extraSpin", bes.extraSpin);
    extract (besObj, "expectedBet", bes.expectedBet);
    extract (besObj, "featureMode", bes.featureMode);
    extract (besObj, "id", bes.id);
    extract (besObj, "costX", bes.costX);

    return bes;
}

FreeSpin freeSpinToNative (Napi::Object& fsObj) {
    FreeSpin fs;

    extract (fsObj, "extraSpins", fs.extraSpins);
    toSpinNative(fsObj, fs);

    return fs;
}

std::vector<FreeSpin> freeSpinsToNative (Napi::Array fssArr) {
    std::vector<FreeSpin> freeSpins;

    if (!fssArr.IsNull() && !fssArr.IsUndefined() && fssArr.IsArray()) {
        for (auto i = 0; i < fssArr.Length(); i++) {
            Napi::Object fsObj = fssArr.Get(i).As<Napi::Object>();
            FreeSpin fs = freeSpinToNative (fsObj);
            freeSpins.push_back (fs);
        }
    }

    return freeSpins;
}

PlayResponse napiPlayResponseToNative(Napi::Object& napiPlayObj) {
    PlayResponse response;

    extract (napiPlayObj, "featureMode", response.featureMode);
    extract (napiPlayObj, "currencyMultiplier", response.currencyMultiplier);
    extract (napiPlayObj, "refBetBase", response.refBetBase);
    extract (napiPlayObj, "betAmount", response.betAmount);
    extract (napiPlayObj, "refBetAmount", response.refBetAmount);
    extract (napiPlayObj, "winAmount", response.winAmount);
    extract (napiPlayObj, "refWinAmount", response.refWinAmount);

    extract (napiPlayObj, "persistent", response.persistent);
    extract (napiPlayObj, "ended", response.ended);
    extract (napiPlayObj, "extraSpin", response.extraSpin);
    extract (napiPlayObj, "empty", response.empty);
    extract (napiPlayObj, "action", response.action);
    extract (napiPlayObj, "subgameTriggered", response.subgameTriggered);

    response.freeSpins          = freeSpinsToNative(napiPlayObj.Get ("freeSpins").As<Napi::Array>());
    response.fsStatus           = toFsStatusNative(napiPlayObj.Get("fsStatus").As<Napi::Array>());
    response.baseSpin           = toBaseSpinNative(napiPlayObj.Get("baseSpin").As<Napi::Object>());
    response.buyExtraSpin       = buyExtraSpinToNative (napiPlayObj.Get("buyExtraSpin").As<Napi::Object>());

    // active features
    Napi::Array activeFeatObj = napiPlayObj.Get ("activeFeatures").As<Napi::Array>();
    for (size_t i = 0; i < activeFeatObj.Length(); i++) {
        std::string featStr = activeFeatObj.Get(i).As<Napi::String>().Utf8Value();
        response.activeFeatures.push_back (FS_FEATURE_FROM_STR (featStr));
    }

    /*
     * Do not copy the previous list of random numbers ... likely we'll not
     * recieve them anyways
     *
    if (napiPlayObj.Has("internal")) {
        Napi::Object internal = napiPlayObj.Get("internal").As<Napi::Object>();
        if (internal.Has("rand")) {
            Napi::Array a = internal.Get("rand").As<Napi::Array>();
            toRHistoryNative(a, response.internal.rHistory);
        }
    }
    */

    return response;
}

RngOpts rngOptsToNative(Napi::Env &env, Napi::Object& obj, const char* libType) {
    RngOpts opts;

    if (!std::string("nats").compare(std::string(libType))) {
        if (!obj.Has("nats"))
            throw std::logic_error("rngOpts must have \"nats\" key");

        Napi::Object natsOptObj = obj.Get("nats").As<Napi::Object>();

        opts.nats.url                  = natsOptObj.Get("url").As<Napi::String>();
        opts.nats.rngCh                = natsOptObj.Get("rngCh").As<Napi::String>();
        opts.nats.timeout              = natsOptObj.Get("timeout").As<Napi::Number>().Int64Value();
        opts.nats.name                 = natsOptObj.Get("name").As<Napi::String>();
        opts.nats.verbose              = natsOptObj.Get("verbose").As<Napi::Boolean>();
        opts.nats.allowReconnect       = natsOptObj.Get("allowReconnect").As<Napi::Boolean>();
        opts.nats.retryOnFailedConnect = natsOptObj.Get("retryOnFailedConnect").As<Napi::Boolean>();
        opts.nats.maxReconnect         = natsOptObj.Get("maxReconnect").As<Napi::Number>().Int64Value();
        opts.nats.reconnectWait        = natsOptObj.Get("reconnectWait").As<Napi::Number>().Int64Value();
        opts.nats.pingInterval         = natsOptObj.Get("pingInterval").As<Napi::Number>().Int64Value();
        opts.nats.maxPingsOut          = natsOptObj.Get("maxPingsOut").As<Napi::Number>().Int64Value();
    }
    else if (!std::string("grpc").compare(std::string(libType))) {
        if (!obj.Has("grpc"))
            throw std::logic_error("rngOpts must have \"grpc\" key");

        Napi::Object grpcOptObj = obj.Get("grpc").As<Napi::Object>();

        opts.grpc.host                 = grpcOptObj.Get("host").As<Napi::String>();
        opts.grpc.port                 = grpcOptObj.Get("port").As<Napi::Number>().Int64Value();
    }

    return opts;
}
