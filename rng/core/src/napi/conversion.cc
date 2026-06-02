#include "./conversion.h"

Napi::Object convertVectoPairToNapi (Napi::Env env, std::vector<uint32_t> vector) {
    unsigned int count = 0;
    Napi::Array arr = Napi::Array::New(env);

    for (auto s : vector) {
        arr.Set(count++, s);
    }

    return arr;
}
