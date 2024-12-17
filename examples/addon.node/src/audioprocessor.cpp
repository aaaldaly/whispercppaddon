#include "audioprocessor.h"
#include <cmath>
#include <cstring>
#include <cstdio>
// Include a WAV loader library such as dr_wav if needed
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

bool AudioProcessor::is_wav_file(const std::vector<uint8_t> &data)
{
    // Minimal check for WAV header 'RIFF' and 'WAVE'
    if (data.size() < 12)
        return false;
    return (std::memcmp(data.data(), "RIFF", 4) == 0 && std::memcmp(data.data() + 8, "WAVE", 4) == 0);
}

bool AudioProcessor::wav_from_buffer(const std::vector<uint8_t> &input, std::vector<float> &pcmf32)
{
    drwav wav;
    if (!drwav_init_memory(&wav, input.data(), input.size(), nullptr))
    {
        fprintf(stderr, "Failed to init WAV from buffer\n");
        return false;
    }

    if (wav.channels != 1 && wav.channels != 2)
    {
        fprintf(stderr, "WAV must be mono or stereo\n");
        drwav_uninit(&wav);
        return false;
    }

    if (wav.sampleRate != 16000)
    {
        fprintf(stderr, "WAV must be 16kHz\n");
        drwav_uninit(&wav);
        return false;
    }

    if (wav.bitsPerSample != 16)
    {
        fprintf(stderr, "WAV must be 16-bit\n");
        drwav_uninit(&wav);
        return false;
    }

    size_t totalFrames = (size_t)wav.totalPCMFrameCount;
    std::vector<int16_t> pcm16(totalFrames * wav.channels);
    drwav_read_pcm_frames_s16(&wav, totalFrames, pcm16.data());

    bool isStereo = (wav.channels == 2);
    pcmf32.resize(totalFrames);
    if (isStereo)
    {
        for (size_t i = 0; i < totalFrames; i++)
        {
            pcmf32[i] = float(pcm16[i]) / 32768.0f;
        }
    }
    else
    {
        // Stereo: combine channels into mono
        for (size_t i = 0; i < totalFrames; i++)
        {
            int32_t val = (int32_t)pcm16[2 * i] + (int32_t)pcm16[2 * i + 1];
            pcmf32[i] = float(val) / (2.0f * 32768.0f);
        }
    }

    return true;
}

bool AudioProcessor::wav_from_stream(const std::vector<uint8_t> &stream, std::vector<float> &pcmf32)
{
    // Here we can implement logic to handle raw audio data that isn't fully WAV
    // For simplicity, assume this transforms raw data to 16-bit 16kHz WAV somehow.
    // The actual implementation depends on your input data format.
    // For now, just return false to indicate not implemented.
    fprintf(stderr, "wav_from_stream not implemented.\n");
    return false;
}

bool AudioProcessor::decode_wav(const std::vector<uint8_t> &data, std::vector<float> &pcmf32)
{
    if (is_wav_file(data))
    {
        return wav_from_buffer(data, pcmf32);
    }
    else
    {
        return wav_from_stream(data, pcmf32);
    }
}
