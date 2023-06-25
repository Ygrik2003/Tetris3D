#include "audio_buffer.h"

audio_buffer::audio_buffer(const char*   path,
                           uint32_t      device_,
                           SDL_AudioSpec audio_spec)
    : device(device_)
{
    SDL_RWops* file = SDL_RWFromFile(path, "rb");
    if (file == nullptr)
    {
        throw std::runtime_error(std::string("can't open audio file: ") + path);
    }

    // freq, format, channels, and samples - used by SDL_LoadWAV_RW
    SDL_AudioSpec file_audio_spec;

    if (SDL_LoadWAV_RW(file, 1, &file_audio_spec, &buffer, &length) == nullptr)
    {
        throw std::runtime_error(std::string("can't load wav: ") + path);
    }

    if (file_audio_spec.channels != audio_spec.channels ||
        file_audio_spec.format != audio_spec.format ||
        file_audio_spec.freq != audio_spec.freq)
    {
        Uint8* output_bytes;
        int    output_length;

        int convert_status = SDL_ConvertAudioSamples(file_audio_spec.format,
                                                     file_audio_spec.channels,
                                                     file_audio_spec.freq,
                                                     buffer,
                                                     static_cast<int>(length),
                                                     audio_spec.format,
                                                     audio_spec.channels,
                                                     audio_spec.freq,
                                                     &output_bytes,
                                                     &output_length);
        if (0 != convert_status)
        {
            throw std::runtime_error(
                std::string("failed to convert WAV byte stream: ") +
                SDL_GetError());
        }

        SDL_free(buffer);
        buffer = output_bytes;
        length = static_cast<uint32_t>(output_length);
    }
    else
    {
        // no need to convert buffer, use as is
    }
}

audio_buffer::~audio_buffer()
{
    SDL_free(buffer);
    // buffer = nullptr;
    length = 0;
}
