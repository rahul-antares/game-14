#pragma once

#include <napi.h>
#include "../game/game.h"
#include "../game/data-defs.h"

Napi::Object createConfigObject(Napi::Env &env, GameConfig& config);
Napi::Array  reelToNapiObject(Napi::Env &env, const std::vector<Symbol> reelSymbols);
Napi::Object playResponseToNAPI(Napi::Env env, const PlayResponse& data);
Napi::Object baseSpinToNAPI(Napi::Env env, const BaseSpin& baseSpin, const bool empty);
Napi::Object spinToNAPI(Napi::Env env, const Spin& Spin, const bool empty);
Napi::Object symCordsVecToNAPI(Napi::Env env, const SymCoords_Vec symbols);
Napi::Object winningToNapiObject(Napi::Env env, const Winning& win);
Napi::Object symCoordsToNAPI(Napi::Env env, const SymCoords& symCoords);
Napi::Object coordinateToNapiObject(Napi::Env env, const Coordinate& coord);
Napi::Object fsStatusToNapi(Napi::Env env, const std::vector<FsStatus>& fsStatus);
Napi::Array  stopsToNAPI(Napi::Env env, const std::array<int, REEL_COUNT>& stops);
Napi::Object freeSpinToNAPI(Napi::Env env, const FreeSpin& freeSpin, const bool empty);
Napi::Object reSpinToNAPI(Napi::Env env, const Spin& reSpin, const bool empty);
Napi::Object gridToNAPI(Napi::Env env, const Grid& grid, bool empty = true);
Napi::Array  stopsToNAPI(Napi::Env env, const std::array<int, REEL_COUNT>& stops, bool empty);
Napi::Object lockedSymbolToNAPI(Napi::Env env, const LockedSym_Vec& lockedSymbols);

PlayResponse napiPlayResponseToNative(Napi::Object& napiPlayObj);
RngOpts      rngOptsToNative(Napi::Env &env, Napi::Object& obj, const char* libType);
