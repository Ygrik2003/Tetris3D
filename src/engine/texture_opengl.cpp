#include "texture_opengl.h"
#include "core/picopng.hxx"
#include "glad/glad.h"

#include <fstream>

void get_pixels_from_png(const char*             path,
                         std::vector<std::byte>& image,
                         unsigned long&          w,
                         unsigned long&          h)
{
    std::vector<std::byte> png_file_in_memory;
    membuff*               file = load_file_to_memory(path);
    png_file_in_memory.resize(file->size);

    std::copy(reinterpret_cast<std::byte*>(file->ptr.get()),
              reinterpret_cast<std::byte*>(file->ptr.get() + file->size),
              png_file_in_memory.begin());

    int error = decodePNG(
        image, w, h, &png_file_in_memory[0], png_file_in_memory.size(), true);

    if (error != 0)
    {
        std::cerr << "error: " << error << std::endl;
        throw std::runtime_error("can't load texture");
    }
}
texture_opengl::texture_opengl(const char* path)
    : file_path(path)
{

    std::vector<std::byte> img;
    unsigned long          w = 0;
    unsigned long          h = 0;
    get_pixels_from_png(path, img, w, h);

    gen_texture_from_pixels(img.data(), w, h);
    // delete file;
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
