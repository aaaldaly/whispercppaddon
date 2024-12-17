
#include "asyncworker.h"
#include "audioprocessor.h"
#include <vector>
#include <cstdint>
#include "napiapi.h"

NapiAPI::NapiAPI() {}

Napi::Value NapiAPI::init_whisper(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsObject())
    {
        Napi::TypeError::New(env, "Expected params object").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object paramsObj = info[0].As<Napi::Object>();

    if (!whisperOp.setup_params(paramsObj, env))
    {
        Napi::Error::New(env, "Failed to setup whisper params").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!whisperOp.init_whisper())
    {
        Napi::Error::New(env, "Failed to init whisper").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Boolean::New(env, true);
}

Napi::Value NapiAPI::free(const Napi::CallbackInfo &info)
{
    whisperOp.free_whisper();
    return info.Env().Undefined();
}

Napi::Value NapiAPI::transcribe(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsTypedArray() || !info[1].IsFunction())
    {
        Napi::TypeError::New(env, "Expected audio data as TypedArray and a callback function").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::TypedArray audioData = info[0].As<Napi::TypedArray>();
    Napi::Function callback = info[1].As<Napi::Function>();

    // log that we are running transcription
    printf("Running transcription\n");
    std::vector<float> audioDataVec = ConvertTypedArrayToVector(audioData);
    (new AsyncWorker(callback, whisperOp, audioDataVec))->Queue();

    return env.Undefined();
}

std::vector<float> NapiAPI::ConvertTypedArrayToVector(const Napi::TypedArray &typedArray)
{
    Napi::Float32Array float32Array = typedArray.As<Napi::Float32Array>();
    std::vector<float> result;
    result.reserve(float32Array.ElementLength());

    for (size_t i = 0; i < float32Array.ElementLength(); ++i)
    {
        result.push_back(float32Array[i]);
    }

    return result;
}