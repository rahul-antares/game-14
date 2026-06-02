#pragma once
#include "symbols.h"
#ifndef EXTRACT_H
#define EXTRACT_H

#include <cstdint>
#include <napi.h>

enum KEY_TYPES { INT, UINT, STRING, DOUBLE, BOOL, NONE };
enum NAPI_OBJ_VAL_TYPE { NA_NULL, NA_UNDEF, NA_NUM, NA_STR, NA_BOOL, NA_ARR, NA_OBJ, NA_INVLD };

//
// ------------- extraction methods
//

void extract (
    const Napi::Object& obj,
    const char* key,
    int& ref,
    const bool required=false,
    const bool boundCheck=false,
    const int min=-1,
    const int max=-1
);

void extract (
    const Napi::Object& obj,
    const char* key,
    int64_t& ref,
    const bool required=false,
    const bool boundCheck=false,
    const int64_t min=-1,
    const int64_t max=-1
);

void extract (
    const Napi::Object& obj,
    const char* key,
    uint32_t& ref,
    const bool required=false,
    const bool boundCheck=false,
    const uint32_t min=0,
    const uint32_t max=0
);

void extract (
    const Napi::Object& obj,
    const char* key,
    double& ref,
    const bool required=false,
    const bool boundCheck=false,
    const double min=-1.0,
    const double max=-1.0
);

void extract (
    const Napi::Object& obj,
    const char* key,
    std::string& ref,
    const bool required=false
);

void extract (
    const Napi::Object& obj,
    const char* key,
    bool& ref,
    const bool required=false
);

//
// ------ helper funcs
//

template <typename T>
void checkBounds (const T value, const T min, const T max, const bool boundCheck);

template <typename T>
T getVal(const Napi::Value& val);

template <>
int64_t getVal<int64_t> (const Napi::Value& val);

template <>
uint32_t getVal<uint32_t> (const Napi::Value& val);

template <>
int32_t getVal<int32_t> (const Napi::Value& val);

template <>
double getVal<double> (const Napi::Value& val);

template <>
std::string getVal<std::string> (const Napi::Value& val);

template <>
bool getVal<bool> (const Napi::Value& val);

std::pair<NAPI_OBJ_VAL_TYPE, std::string> getValueType(const Napi::Value& val);

//
// ------ array methods
//

void getArray (const Napi::Value& val, std::vector<std::string>& strArr);
void getArray (const Napi::Value& val, std::vector<int32_t>& int32Arr);
void getArray (const Napi::Value& val, std::vector<int64_t>& int64Arr);
void getArray (const Napi::Value& val, std::vector<double>& doubleArr);
void getArray (const Napi::Value& val, std::vector<bool>& boolArr);
void getArray (const Napi::Value& val, std::vector<uint32_t>& uint32Arr);

//
//------- set  as napi methods
//

template<typename T, typename Func>
void setValueHelper( Napi::Object& obj, const char* key, const T& value, const char* typeStr, Func creator);

void setValue(Napi::Object& obj, const char* key, bool value);
void setValue(Napi::Object& obj, const char* key, int32_t value);
void setValue(Napi::Object& obj, const char* key, uint32_t value);
void setValue(Napi::Object& obj, const char* key, int64_t value);
void setValue(Napi::Object& obj, const char* key, uint64_t value);
void setValue(Napi::Object& obj, const char* key, double value);
void setValue(Napi::Object& obj, const char* key, const char* value);
void setValue(Napi::Object& obj, const char* key, const std::string& value);

//
// ----------- napi return value methods
//

void printObjKeys(Napi::Object obj, std::string message = "", int indent = 0);

Napi::String  symbolToNapi   (Napi::Env& env, const Symbol& sym);
Napi::String  stringToNapi   (Napi::Env& env, const char* str);
Napi::String  stringToNapi   (Napi::Env& env, const std::string& str);
Napi::Number  intToNapi      (Napi::Env& env, const int64_t& val);
Napi::Number  intToNapi      (Napi::Env& env, const int32_t& val);
Napi::Number  uIntToNapi     (Napi::Env& env, const uint64_t& val);
Napi::Number  doubleToNapi   (Napi::Env& env, const double& val);
Napi::Boolean boolToNapi     (Napi::Env& env, const bool& val);
Napi::Array   vecToNapiArray (Napi::Env& env, const std::vector<std::string>& strs);
Napi::Array   vecToNapiArray (Napi::Env env,  const std::vector<Symbol>& vec);
Napi::Array   vecToNapiArray (Napi::Env env,  const std::vector<int64_t>& vec);
Napi::Array   vecToNapiArray (Napi::Env env,  const std::vector<uint64_t>& vec);
Napi::Array   vecToNapiArray (Napi::Env env,  const std::vector<bool>& vec);
Napi::Array   vecToNapiArray (Napi::Env env,  const std::vector<double>& vec);

#endif // EXTRACT_H
