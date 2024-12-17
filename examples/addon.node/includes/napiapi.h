#pragma once
#include <napi.h>
#include "whisperoperator.h"

class NapiAPI
{
public:
    NapiAPI();

    // Takes params from JS (Napi object), sets up whisper, returns success
    Napi::Value init_whisper(const Napi::CallbackInfo &info);

    // Frees whisper resources
    Napi::Value free(const Napi::CallbackInfo &info);

    // Starts transcription (async)
    Napi::Value transcribe(const Napi::CallbackInfo &info);

private:
    WhisperOperator whisperOp;

    std::vector<float> ConvertTypedArrayToVector(const Napi::TypedArray &typedArray);
};