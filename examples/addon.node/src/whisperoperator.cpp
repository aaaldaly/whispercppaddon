#include "whisperoperator.h"
#include <cstdio>
#include <thread>
#include <algorithm>

WhisperOperator::WhisperOperator() : ctx(nullptr), initialized(false) {}

WhisperOperator::~WhisperOperator()
{
    free_whisper();
}

bool WhisperOperator::setup_params(Napi::Object &paramsObj, Napi::Env env)
{
    whisper_params wp;

    if (!paramsObj.Has("language") || !paramsObj.Get("language").IsString())
    {
        Napi::TypeError::New(env, "Expected 'language' to be a string and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.language = paramsObj.Get("language").As<Napi::String>();

    if (!paramsObj.Has("model") || !paramsObj.Get("model").IsString())
    {
        Napi::TypeError::New(env, "Expected 'model' to be a string and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.model = paramsObj.Get("model").As<Napi::String>();

    if (!paramsObj.Has("use_gpu") || !paramsObj.Get("use_gpu").IsBoolean())
    {
        Napi::TypeError::New(env, "Expected 'use_gpu' to be a boolean and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.use_gpu = paramsObj.Get("use_gpu").As<Napi::Boolean>();

    if (!paramsObj.Has("flash_attn") || !paramsObj.Get("flash_attn").IsBoolean())
    {
        Napi::TypeError::New(env, "Expected 'flash_attn' to be a boolean and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.flash_attn = paramsObj.Get("flash_attn").As<Napi::Boolean>();

    if (!paramsObj.Has("no_prints") || !paramsObj.Get("no_prints").IsBoolean())
    {
        Napi::TypeError::New(env, "Expected 'no_prints' to be a boolean and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.no_prints = paramsObj.Get("no_prints").As<Napi::Boolean>();

    if (!paramsObj.Has("no_timestamps") || !paramsObj.Get("no_timestamps").IsBoolean())
    {
        Napi::TypeError::New(env, "Expected 'no_timestamps' to be a boolean and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.no_timestamps = paramsObj.Get("no_timestamps").As<Napi::Boolean>();

    if (!paramsObj.Has("audio_ctx") || !paramsObj.Get("audio_ctx").IsNumber())
    {
        Napi::TypeError::New(env, "Expected 'audio_ctx' to be a number and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.audio_ctx = paramsObj.Get("audio_ctx").As<Napi::Number>();

    if (!paramsObj.Has("comma_in_time") || !paramsObj.Get("comma_in_time").IsBoolean())
    {
        Napi::TypeError::New(env, "Expected 'comma_in_time' to be a boolean and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.comma_in_time = paramsObj.Get("comma_in_time").As<Napi::Boolean>();

    if (!paramsObj.Has("n_threads") || !paramsObj.Get("n_threads").IsNumber())
    {
        Napi::TypeError::New(env, "Expected 'n_threads' to be a number and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.n_threads = paramsObj.Get("n_threads").As<Napi::Number>();

    if (!paramsObj.Has("n_processors") || !paramsObj.Get("n_processors").IsNumber())
    {
        Napi::TypeError::New(env, "Expected 'n_processors' to be a number and provided").ThrowAsJavaScriptException();
        return false;
    }
    wp.n_processors = paramsObj.Get("n_processors").As<Napi::Number>();

    // If pcmf32 provided as TypedArray in params
    Napi::Value pcmf32Value = paramsObj.Get("pcmf32");
    if (pcmf32Value.IsTypedArray())
    {
        Napi::Float32Array arr = pcmf32Value.As<Napi::Float32Array>();
        wp.pcmf32.reserve(arr.ElementLength());
        for (size_t i = 0; i < arr.ElementLength(); i++)
        {
            wp.pcmf32.push_back(arr[i]);
        }
    }

    this->current_params = wp;
    return true;
}

bool WhisperOperator::init_whisper()
{
    if (initialized)
    {
        return true;
    }
    // Initialize whisper context
    struct whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = current_params.use_gpu;
    cparams.flash_attn = current_params.flash_attn;
    ctx = whisper_init_from_file_with_params(current_params.model.c_str(), cparams);
    if (!ctx)
    {
        fprintf(stderr, "Failed to init whisper context\n");
        return false;
    }
    initialized = true;
    return true;
}

void WhisperOperator::free_whisper()
{
    if (ctx)
    {
        whisper_free(ctx);
        ctx = nullptr;
    }
    initialized = false;
}

void WhisperOperator::print_logs()
{
    if (!initialized)
    {
        fprintf(stderr, "Whisper not initialized.\n");
        return;
    }
    fprintf(stderr, "Model: %s\n", current_params.model.c_str());
    fprintf(stderr, "Language: %s\n", current_params.language.c_str());
    fprintf(stderr, "Threads: %d\n", current_params.n_threads);
    // Add other prints as needed
}

std::string WhisperOperator::run_whisper_transcription(const std::vector<float> &pcmf32)
{
    // Similar logic to what was in the original code's whisper_transcribe
    // Create whisper_full_params, run whisper_full, and return transcription result.
    // This is simplified pseudocode:
    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.print_progress = !current_params.no_prints;
    wparams.print_timestamps = !current_params.no_timestamps;
    wparams.translate = false;
    wparams.language = current_params.language.c_str();
    wparams.n_threads = current_params.n_threads;
    wparams.no_timestamps = current_params.no_timestamps;

    if (whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size()) != 0)
    {
        fprintf(stderr, "Failed to run transcription\n");
        return "";
    }

    // Collect transcription result
    int n_segments = whisper_full_n_segments(ctx);
    std::string result;
    for (int i = 0; i < n_segments; ++i)
    {
        const char *text = whisper_full_get_segment_text(ctx, i);
        result += text;
        result += " ";
    }
    return result;
}

std::string WhisperOperator::transcript(const std::vector<float> &audioData)
{
    // TODO: call the from sterio to mono function directlry and test
    // TODO: implement audio inference and transformation for mic streams
    if (!initialized)
    {
        fprintf(stderr, "Whisper not initialized.\n");
        return "-";
    }
    return run_whisper_transcription(audioData);
}