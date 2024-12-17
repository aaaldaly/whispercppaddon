#pragma once
#include <string>
#include <vector>
#include "whisper.h" // Include your whisper headers
#include "common.h"  // If needed
#include <napi.h>
#include "audioprocessor.h"

struct whisper_params
{
    int32_t n_threads;
    int32_t n_processors;
    int32_t audio_ctx;
    bool use_gpu;
    bool flash_attn;
    bool no_prints;
    bool no_timestamps;
    bool diarize;
    bool comma_in_time;
    std::string language;
    std::string model;
    std::vector<float> pcmf32;
};

class WhisperOperator
{
public:
    WhisperOperator();
    ~WhisperOperator();

    // set and validate params, set defaults if needed
    bool setup_params(Napi::Object &params, Napi::Env env);

    // init whisper using params
    bool init_whisper();

    // free whisper context
    void free_whisper();

    // print current params and context info
    void print_logs();

    // run transcription on the provided buffer (assumes pcmf32 ready)
    std::string transcript(const std::vector<float> &audioData);

private:
    whisper_context *ctx;
    whisper_params current_params;
    AudioProcessor audio_processor;
    bool initialized;

    // internal helper functions from your previous code
    // e.g. a helper to run whisper_full and return result
    std::string run_whisper_transcription(const std::vector<float> &pcmf32);
};