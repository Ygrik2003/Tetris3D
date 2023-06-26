#include "texture_opengl.h"
#include "core/picopng.hxx"
#include "glad/glad.h"

#include <fstream>
#include <vector>

texture_opengl::texture_opengl(const char* path)
    : file_path(path)
{
    std::vector<std::byte> png_file_in_memory;

    // std::ifstream          ifs(path, std::ios_base::binary);
    // if (!ifs)
    // {
    //     throw std::runtime_error("can't load texture");
    // }
    // ifs.seekg(0, std::ios_base::end);
    // std::streamoff pos_in_file = ifs.tellg();
    // png_file_in_memory.resize(static_cast<size_t>(pos_in_file));
    // ifs.seekg(0, std::ios_base::beg);
    // if (!ifs)
    // {
    //     throw std::runtime_error("can't load texture");
    // }

    // ifs.read(reinterpret_cast<char*>(png_file_in_memory.data()),
    // pos_in_file); if (!ifs.good())
    // {
    //     throw std::runtime_error("can't load texture");
    // }

    membuff* file = load_file_to_memory(path);
    png_file_in_memory.resize(file->size);

    std::copy(reinterpret_cast<std::byte*>(file->ptr.get()),
              reinterpret_cast<std::byte*>(file->ptr.get() + file->size),
              png_file_in_memory.begin());

    std::vector<std::byte> image;
    unsigned long          w     = 0;
    unsigned long          h     = 0;
    int                    error = decodePNG(
        image, w, h, &png_file_in_memory[0], png_file_in_memory.size(), true);

    if (error != 0)
    {
        std::cerr << "error: " << error << std::endl;
        throw std::runtime_error("can't load texture");
    }

    gen_texture_from_pixels(image.data(), w, h);
    delete file;
}

texture_opengl::texture_opengl(const void*  pixels,
                               const size_t width,
                               const size_t height)
{
    gen_texture_from_pixels(pixels, width, height);
}

texture_opengl::~texture_opengl()
{
    glDeleteTextures(1, &handle);
    GL_CHECK_ERRORS()
}

void texture_opengl::bind() const
{
    glBindTexture(GL_TEXTURE_2D, handle);
    GL_CHECK_ERRORS()
}

void texture_opengl::gen_texture_from_pixels(const void*  pixels,
                                             const size_t width,
                                             const size_t height)
{
    glGenTextures(1, &handle);
    GL_CHECK_ERRORS()
    glBindTexture(GL_TEXTURE_2D, handle);
    GL_CHECK_ERRORS()

    GLint   mipmap_level = 0;
    GLint   border       = 0;
    GLsizei width_       = static_cast<GLsizei>(width);
    GLsizei height_      = static_cast<GLsizei>(height);
    glTexImage2D(GL_TEXTURE_2D,
                 mipmap_level,
                 GL_RGBA,
                 width_,
                 height_,
                 border,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels);
    GL_CHECK_ERRORS()

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CHECK_ERRORS()
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK_ERRORS()
}
