#include <cstdint>
#include <iostream>
#include <cstddef>
#include <vector>
#include <string.h>
#include "./extract.h"
#include "./error.h"
#include "console.h"
#include "napi.h"

//
// ------------- extraction methods
//

void extract (
    const Napi::Object& obj,
    const char* key,
    int& ref,
    const bool required,
    const bool boundCheck,
    const int min,
    const int max
) {
    Napi::Value val         = obj.Get(key);
    auto [valType, typeStr] = getValueType(val);

    if (valType != NA_NUM) {
        if (required)
            throw_error("Expected Integer for ", key, ", found ", typeStr);

        return;
    }

    ref = val.As<Napi::Number>().Int64Value();
    checkBounds(ref, min, max, boundCheck);
}

void extract (
    const Napi::Object& obj,
    const char* key,
    int64_t& ref,
    const bool required,
    const bool boundCheck,
    const int64_t min,
    const int64_t max
) {
    Napi::Value val         = obj.Get(key);
    auto [valType, typeStr] = getValueType(val);

    if (valType != NA_NUM) {
        if (required)
            throw_error("Expected Integer(64) for ", key, ", found ", typeStr);

        return;
    }

    ref = val.As<Napi::Number>().Int64Value();
    checkBounds (ref, min, max, boundCheck);
}

void extract (
    const Napi::Object& obj,
    const char* key,
    uint32_t& ref,
    const bool required,
    const bool boundCheck,
    const uint32_t min,
    const uint32_t max
) {
    Napi::Value val         = obj.Get(key);
    auto [valType, typeStr] = getValueType(val);

    if (valType != NA_NUM && required)
        throw_error("Expected Non-Negative Integer(32) for ", key, ", found ", typeStr);

    if (valType != NA_NUM)
        return;

    ref = val.As<Napi::Number>().Uint32Value();
    checkBounds (ref, min, max, boundCheck);
}

void extract (
    const Napi::Object& obj,
    const char* key,
    bool& ref,
    const bool required
) {
    Napi::Value val         = obj.Get(key);
    auto [valType, typeStr] = getValueType(val);

    if (valType != NA_BOOL) {
        if (required)
            throw_error("Expected Boolean for ", key, ", found ", typeStr);

        return;
    }

    ref = val.As<Napi::Boolean>();
}

void extract (
    const Napi::Object& obj,
    const char* key,
    std::string& ref,
    const bool required
) {
    Napi::Value val         = obj.Get(key);
    auto [valType, typeStr] = getValueType(val);

    if (valType != NA_STR) {
        if (required)
            throw_error("Expected String for ", key, ", found ", typeStr);

        return;
    }

    ref = val.As<Napi::String>().Utf8Value();
}

void extract (
    const Napi::Object& obj,
    const char* key,
    double& ref,
    const bool required,
    const bool boundCheck,
    const double min,
    const double max
) {
    Napi::Value val         = obj.Get(key);
    auto [valType, typeStr] = getValueType(val);

    if (valType != NA_NUM) {
        if (required)
            throw_error("Expected Double for ", key, ", found ", typeStr);

        return;
    }

    ref = val.As<Napi::Number>().DoubleValue();
    checkBounds (ref, min, max, boundCheck);
}

//
// ------ helper funcs
//

std::pair<NAPI_OBJ_VAL_TYPE, std::string> getValueType(const Napi::Value& val) {
    enum NAPI_OBJ_VAL_TYPE valType = NA_INVLD;
    std::string valTypeErrMsg      = "invalid value";

    if (val.IsNull()) {
        valType = NA_NULL;
        valTypeErrMsg = "Null";
    } else if (val.IsUndefined()) {
        valType = NA_UNDEF;
        valTypeErrMsg = "Undefined";
    } else if (val.IsString()) {
        valType = NA_STR;
        valTypeErrMsg = "String";
    } else if (val.IsNumber()) {
        valType = NA_NUM;
        valTypeErrMsg = "Number";
    } else if (val.IsBoolean()) {
        valType = NA_BOOL;
        valTypeErrMsg = "Boolean";
    } else if (val.IsArray()) {
        valType = NA_ARR;
        valTypeErrMsg = "Array";
    } else if (val.IsObject()) {
        valType = NA_OBJ;
        valTypeErrMsg = "Object";
    }

    return std::make_pair(valType, valTypeErrMsg);
}

template <typename T>
T getVal(const Napi::Value& val);

template <>
int64_t getVal<int64_t> (const Napi::Value& val) {
    return val.As<Napi::Number>().Int64Value();
}

template <>
int32_t getVal<int32_t> (const Napi::Value& val) {
    return val.As<Napi::Number>().Int32Value();
}

template <>
uint32_t getVal<uint32_t> (const Napi::Value& val) {
    return val.As<Napi::Number>().Uint32Value();
}

template <>
double getVal<double> (const Napi::Value& val) {
    return val.As<Napi::Number>().DoubleValue();
}

template <>
std::string getVal<std::string> (const Napi::Value& val) {
    return val.As<Napi::String>().Utf8Value();
}

template <>
bool getVal<bool> (const Napi::Value& val) {
    return val.As<Napi::Boolean>();
}

template <typename T>
void checkBounds (const T value, const T min, const T max, const bool boundCheck) {
    static_assert(std::is_arithmetic<T>::value, "expected an arithmetic type(int, float/double)");

    if (!boundCheck) return;

    if (value < min || value > max)
        throw_error("Value(", value, ") is out of range [", min, ", ", max, "].");
}

//
// ------ array methods
//

void getArray (const Napi::Value& val, std::vector<std::string>& strArr) {
    auto [valType, typeStr] = getValueType(val);
    if (valType != NA_ARR)
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        std::string str = getVal<std::string>(objArr.Get(i));
        strArr.push_back(str);
    }
}

void getArray (const Napi::Value& val, std::vector<int32_t>& int32Arr) {
    auto [valType, typeStr] = getValueType(val);
    if (valType != NA_ARR)
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        int32_t inVal = getVal<int32_t>(objArr.Get(i));
        int32Arr.push_back(inVal);
    }
}

void getArray (const Napi::Value& val, std::vector<int64_t>& int64Arr) {
    auto [valType, typeStr] = getValueType(val);
    if (valType != NA_ARR)
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        int64_t inVal = getVal<int64_t>(objArr.Get(i));
        int64Arr.push_back(inVal);
    }
}

void getArray (const Napi::Value& val, std::vector<double>& doubleArr) {
    auto [valType, typeStr] = getValueType(val);
    if (valType != NA_ARR)
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        double dVal = getVal<double>(objArr.Get(i));
        doubleArr.push_back(dVal);
    }
}

void getArray (const Napi::Value& val, std::vector<bool>& boolArr) {
    auto [valType, typeStr] = getValueType(val);
    if (valType != NA_ARR)
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        bool bVal = getVal<bool>(objArr.Get(i));
        boolArr.push_back(bVal);
    }
}

void getArray (const Napi::Value& val, std::vector<uint32_t>& uint32Arr) {
    auto [valType, typeStr] = getValueType(val);
    if (valType != NA_ARR)
        return;

    Napi::Array objArr = val.As<Napi::Array>();
    for (auto i = 0; i < objArr.Length(); i++) {
        uint32_t uVal = getVal<uint32_t>(objArr.Get(i));
        uint32Arr.push_back(uVal);
    }
}


//
// ---- set as napi type
//

template<typename T, typename Func>
void setValueHelper(
    Napi::Object& obj,
    const char* key,
    const T& value,
    const char* typeStr,
    Func creator
) {
    try {
        Napi::Env env = obj.Env();
        obj.Set(key, creator(env, value));
    } catch (const std::exception& e) {
        throw_error("Failed to set ", typeStr, " value for key '", key, "': ", e.what());
    }
}

void setValue(Napi::Object& obj, const char* key, const bool value) {
    setValueHelper(
        obj, key,
        value, "boolean",
        [](Napi::Env env, bool v) {
            return Napi::Boolean::New(env, v);
        }
    );
}

void setValue(Napi::Object& obj, const char* key, const int32_t value) {
    setValueHelper(
        obj, key,
        value, "int32",
        [](Napi::Env env, int32_t v) {
            return Napi::Number::New(env, v);
        }
    );
}

void setValue(Napi::Object& obj, const char* key, const uint32_t value) {
    setValueHelper(
        obj, key,
        value, "uint32",
        [](Napi::Env env, uint32_t v) {
            return Napi::Number::New(env, v);
        }
    );
}

void setValue(Napi::Object& obj, const char* key, const int64_t value) {
    setValueHelper(
        obj, key,
        value , "int64",
        [](Napi::Env env, int64_t v) {
            return Napi::Number::New(env, v);
        }
    );
}

void setValue(Napi::Object& obj, const char* key, const uint64_t value) {
    setValueHelper(
        obj, key,
        value, "uint64",
        [](Napi::Env env, uint64_t v) {
            return Napi::Number::New(env, v);
        }
    );
}

void setValue(Napi::Object& obj, const char* key, const double value) {
    setValueHelper(
        obj, key,
        value, "double",
        [](Napi::Env env, double v) {
            return Napi::Number::New(env, v); 
        }
    );
}

void setValue(Napi::Object& obj, const char* key, const char* value) {
    setValueHelper(
        obj, key,
        value, "string",
        [](Napi::Env env, const char* v) {
            return Napi::String::New(env, v);
        }
    );
}

void setValue(Napi::Object& obj, const char* key, const std::string& value) {
    setValueHelper(
        obj, key,
        value, "string",
        [](Napi::Env env, const std::string& v) {
            return Napi::String::New(env, v);
        }
    );
}


//
// ----------- napi return value methods
//

Napi::String symbolToNapi(Napi::Env& env, const Symbol& sym) {
    return Napi::String::New(env, SymbolChar[sym]);
}

Napi::String stringToNapi (Napi::Env& env, const char* str) {
    return Napi::String::New(env, str);
}

Napi::String stringToNapi (Napi::Env& env, const std::string& str) {
    return Napi::String::New(env, str);
}

Napi::Number intToNapi(Napi::Env& env, const int64_t& val) {
    return Napi::Number::New(env, val);
}

Napi::Number intToNapi(Napi::Env& env, const int32_t& val) {
    return Napi::Number::New(env, val);
}

Napi::Number uIntToNapi(Napi::Env& env, const uint64_t& val) {
    return Napi::Number::New(env, val);
}

Napi::Number doubleToNapi(Napi::Env& env, const double& val) {
    return Napi::Number::New(env, val);
}

Napi::Boolean boolToNapi(Napi::Env& env, const bool& val) {
    return Napi::Boolean::New(env, val);
}

Napi::Array vecToNapiArray (Napi::Env& env, const std::vector<std::string>& strs) {
    Napi::Array arr = Napi::Array::New(env, strs.size());

    for (auto i = 0; i < strs.size(); i++) {
        arr.Set(i, stringToNapi(env, strs[i]));
    }

    return arr;
}

Napi::Array vecToNapiArray( Napi::Env env, const std::vector<Symbol>& vec) {
    Napi::Array arr = Napi::Array::New(env, vec.size());

    for (size_t i = 0; i < vec.size(); ++i) {
        arr.Set(i, symbolToNapi(env, vec[i]));
    }

    return arr;
}

Napi::Array vecToNapiArray( Napi::Env env, const std::vector<int64_t>& vec) {
    Napi::Array arr = Napi::Array::New(env, vec.size());

    for (size_t i = 0; i < vec.size(); ++i) {
        arr.Set(i, intToNapi(env, vec[i]));
    }

    return arr;
}

Napi::Array vecToNapiArray( Napi::Env env, const std::vector<uint64_t>& vec) {
    Napi::Array arr = Napi::Array::New(env, vec.size());

    for (size_t i = 0; i < vec.size(); ++i) {
        arr.Set(i, uIntToNapi(env, vec[i]));
    }

    return arr;
}

Napi::Array vecToNapiArray( Napi::Env env, const std::vector<bool>& vec) {
    Napi::Array arr = Napi::Array::New(env, vec.size());

    for (size_t i = 0; i < vec.size(); ++i) {
        arr.Set(i, boolToNapi(env, vec[i]));
    }

    return arr;
}

Napi::Array vecToNapiArray( Napi::Env env, const std::vector<double>& vec) {
    Napi::Array arr = Napi::Array::New(env, vec.size());

    for (size_t i = 0; i < vec.size(); ++i) {
        arr.Set(i, doubleToNapi(env, vec[i]));
    }

    return arr;
}


void printObjKeys(Napi::Object obj, std::string message, int indent) {
    std::string indentStr(indent * 2, ' ');

    if (indent == 0)
        console::log("{}", message);

    Napi::Array props = obj.GetPropertyNames();
    uint32_t length = props.Length();

    for (uint32_t i = 0; i < length; i++) {
        std::string key = props.Get(i).As<Napi::String>().Utf8Value();
        Napi::Value val = obj.Get(key);

        if (val.IsNull()) {
            console::log ("{}  {} : [null]", indentStr, key);
        } else if (val.IsUndefined()) {
            console::log ("{}  {} : [undefined]", indentStr, key);
        } else if (val.IsBoolean()) {
            console::log ("{}  {} : {}", indentStr, key, val.As<Napi::Boolean>().Value());
        } else if (val.IsNumber()) {
            double num = val.As<Napi::Number>().DoubleValue();
            console::log ("{}  {} : {}", indentStr, key, num);
        } else if (val.IsString()) {
            console::log ("{}  {} : \"{}\"", indentStr, key, val.As<Napi::String>().Utf8Value());
        } else if (val.IsArray()) {
            Napi::Array arr = val.As<Napi::Array>();
            console::log ("{}  {} : [Array, length: {}]", indentStr, key, arr.Length());
        } else if (val.IsObject()) {
            Napi::Object nestedObj = val.As<Napi::Object>();
            Napi::Array nestedProps = nestedObj.GetPropertyNames();
            console::log ("{}  {} : [Object, {} key(s)]", indentStr, key, nestedProps.Length());
        } else if (val.IsFunction()) {
            console::log ("{}  {} : [Function]", indentStr, key);
        } else {
            console::log ("{}  {} : [Unknown Type]", indentStr, key);
        }
    }
}

