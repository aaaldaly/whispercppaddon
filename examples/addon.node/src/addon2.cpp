#include <napi.h>
#include "napiapi.h"

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    // Create an instance of NapiAPI to hold references
    static NapiAPI api;

    exports.Set(Napi::String::New(env, "init_whisper"),
                Napi::Function::New(env, [](const Napi::CallbackInfo &info)
                                    { return api.init_whisper(info); }));

    exports.Set(Napi::String::New(env, "free"),
                Napi::Function::New(env, [](const Napi::CallbackInfo &info)
                                    { return api.free(info); }));

    exports.Set(Napi::String::New(env, "transcribe"),
                Napi::Function::New(env, [](const Napi::CallbackInfo &info)
                                    { return api.transcribe(info); }));

    return exports;
}

NODE_API_MODULE(addon, Init);