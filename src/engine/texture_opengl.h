#pragma once
#include "core/types.h"
#include "texture.h"

class texture_opengl : public texture
{
public:
    texture_opengl(const char* path);
    texture_opengl(const void* pixels, const size_t width, const size_t height);
    ~texture_opengl() override;

    void bind() const override;

    uint32_t get_width() const override { return width; }
    uint32_t get_height() const override { return height; }

private:
    void gen_texture_from_pixels(const void*  pixels,
                                 const size_t width,
                                 const size_t height);

    const char* file_path;
    uint32_t    handle = 0;
    uint32_t    width  = 0;
    uint32_t    height = 0;
};