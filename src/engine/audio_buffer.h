#pragma once
#include "SDL3/SDL.h"
#include <memory>
#include <mutex>

#pragma pack(push, 1)
class audio_buffer
{
public:
    audio_buffer(const char* path, uint32_t device, SDL_AudioSpec audio_spec);
    ~audio_buffer();

    // std::unique_ptr<uint8_t*> tmp_buf;
    uint8_t*          buffer        = nullptr;
    uint32_t          length        = 0;
    uint32_t          current_index = 0;
    SDL_AudioDeviceID device;
    bool              is_playing = false;
    bool              is_looped  = false;
};
#pragma pack(pop)
