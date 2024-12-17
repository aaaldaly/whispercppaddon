#pragma once
#include <vector>
#include <cstdint>

class AudioProcessor
{
public:
    // Converts raw audio buffer data (which already includes WAV header)
    // into a normalized 16-bit 16kHz WAV buffer.
    bool wav_from_buffer(const std::vector<uint8_t> &input, std::vector<float> &pcmf32);

    // Converts a raw audio stream (which might not have a full WAV header)
    // into a 16-bit 16kHz WAV buffer.
    bool wav_from_stream(const std::vector<uint8_t> &stream, std::vector<float> &pcmf32);

    // Checks the provided data, determines if it's a full WAV or a raw stream,
    // then decodes accordingly by calling wav_from_buffer or wav_from_stream.
    bool decode_wav(const std::vector<uint8_t> &data, std::vector<float> &pcmf32);

    // Converts stereo WAV PCM float data into mono (averages the two channels).
    void stereo_to_mono(const std::vector<float> &inputStereo, std::vector<float> &outputMono);

private:
    bool is_wav_file(const std::vector<uint8_t> &data);
    // Add other private helper functions if needed.
};