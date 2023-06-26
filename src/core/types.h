#pragma once
#include <array>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.141592653589793238
#endif
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>

#define GL_CHECK_ERRORS()                                                      \
    {                                                                          \
        const GLenum err = glGetError();                                       \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            switch (err)                                                       \
            {                                                                  \
                case GL_INVALID_ENUM:                                          \
                    std::cerr << "GL_INVALID_ENUM" << std::endl;               \
                    break;                                                     \
                case GL_INVALID_VALUE:                                         \
                    std::cerr << "GL_INVALID_VALUE" << std::endl;              \
                    break;                                                     \
                case GL_INVALID_OPERATION:                                     \
                    std::cerr << "GL_INVALID_OPERATION" << std::endl;          \
                    break;                                                     \
                case GL_INVALID_FRAMEBUFFER_OPERATION:                         \
                    std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION"            \
                              << std::endl;                                    \
                    break;                                                     \
                case GL_OUT_OF_MEMORY:                                         \
                    std::cerr << "GL_OUT_OF_MEMORY" << std::endl;              \
                    break;                                                     \
            }                                                                  \
            std::cerr << __FILE__ << ':' << __LINE__ << '(' << __FUNCTION__    \
                      << ')' << std::endl;                                     \
            assert(false);                                                     \
        }                                                                      \
    }

struct uniform
{
    float alpha = 0.f; // For animation

    float width  = 0.f; // Resolution
    float height = 0.f;

    float* rotate_alpha_obj; // Rotate object
    float* rotate_beta_obj;
    float* rotate_gamma_obj;

    float* rotate_alpha_camera; // Rotate camera
    float* rotate_beta_camera;
    float* rotate_gamma_camera;

    float* translate_x_obj; // Translate object
    float* translate_y_obj;
    float* translate_z_obj;

    float* translate_x_camera; // Translate camera
    float* translate_y_camera;
    float* translate_z_camera;

    float* scale_x_obj; // Scale object
    float* scale_y_obj;
    float* scale_z_obj;
};

struct vertex3d;
struct vertex3d_colored;
struct vertex3d_textured;
struct vertex3d_colored_textured;

struct vector2d
{
    vector2d();
    vector2d(float x, float y);

    vector2d  operator+(const vector2d& right);
    vector2d  operator+=(const vector2d& right);
    vector2d  operator-(const vector2d& right);
    vector2d& operator=(const vector2d& right);
    float     length() const;

    vector2d& normalize();

    float x = 0.;
    float y = 0.;
};

struct vector3d
{
    vector3d();
    vector3d(float x, float y, float z);

    vector3d        operator+(const vector3d& right);
    vector3d        operator+=(const vector3d& right);
    vector3d        operator-(const vector3d& right);
    vector3d        operator*(const float& right);
    vector3d        operator/(const float& right);
    friend vector3d operator*(const float& left, const vector3d& right);
    vector3d&       operator=(const vector3d& right);
    float           length() const;

    vector3d& normalize();

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

struct vector4d
{
    vector4d();
    vector4d(float x, float y, float z, float w);

    vector4d  operator+(const vector4d& right);
    vector4d  operator+=(const vector4d& right);
    vector4d  operator-(const vector4d& right);
    vector4d& operator=(const vector4d& right);
    float     length() const;

    vector4d& normalize();

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;
};

namespace color
{
class rgba
{
public:
    rgba();
    rgba(float r, float g, float b, float a);

    float get_r() const
    {
        std::uint32_t r_ = (value & 0x000000FF) >> 0;
        return r_ / 255.f;
    }
    float get_g() const
    {
        std::uint32_t g_ = (value & 0x0000FF00) >> 8;
        return g_ / 255.f;
    }
    float get_b() const
    {
        std::uint32_t b_ = (value & 0x00FF0000) >> 16;
        return b_ / 255.f;
    }
    float get_a() const
    {
        std::uint32_t a_ = (value & 0xFF000000) >> 24;
        return a_ / 255.f;
    }
    void set_r(const float r)
    {
        std::uint32_t r_ = static_cast<std::uint32_t>(r * 255);
        value &= 0xFFFFFF00;
        value |= (r_ << 0);
    }
    void set_g(const float g)
    {
        std::uint32_t g_ = static_cast<std::uint32_t>(g * 255);
        value &= 0xFFFF00FF;
        value |= (g_ << 8);
    }
    void set_b(const float b)
    {
        std::uint32_t b_ = static_cast<std::uint32_t>(b * 255);
        value &= 0xFF00FFFF;
        value |= (b_ << 16);
    }
    void set_a(const float a)
    {
        std::uint32_t a_ = static_cast<std::uint32_t>(a * 255);
        value &= 0x00FFFFFF;
        value |= a_ << 24;
    }

private:
    uint32_t value = 0;
};
} // namespace color

struct vertex2d
{
    vertex2d();
    vertex2d(float x, float y);

    vector2d pos;

    static const uint8_t OFFSET_POSITION = 0;
    // static const uint8_t OFFSET_NORMAL   = 0;
};

struct vertex3d
{
    vertex3d();
    vertex3d(float x, float y, float z);

    vector3d pos;
    vector3d normal;

    static const uint8_t OFFSET_POSITION = 0;
    static const uint8_t OFFSET_NORMAL   = sizeof(vector3d);
};

struct vertex2d_colored : vertex2d
{
    vertex2d_colored();
    vertex2d_colored(vertex2d ver, color::rgba color);

    color::rgba rgba;

    static const uint8_t OFFSET_COLOR = sizeof(vertex2d);
};

struct vertex3d_colored : vertex3d
{
    vertex3d_colored();
    vertex3d_colored(vertex3d ver, color::rgba color);

    color::rgba rgba;

    static const uint8_t OFFSET_COLOR = sizeof(vertex3d);
};

struct vertex2d_textured : vertex2d
{
    vertex2d_textured();
    vertex2d_textured(vertex2d ver, vector2d uv);

    vector2d uv;

    static const uint8_t OFFSET_TEXTURE = sizeof(vertex2d);
};
struct vertex3d_textured : vertex3d
{
    vertex3d_textured();
    vertex3d_textured(vertex3d ver, vector2d uv);

    vector2d uv;

    static const uint8_t OFFSET_TEXTURE = sizeof(vertex3d);
};

struct vertex2d_colored_textured : vertex2d
{
    vertex2d_colored_textured();
    vertex2d_colored_textured(vertex2d ver, color::rgba clr, vector2d uv);

    color::rgba rgba;
    vector2d    uv;

    static const uint8_t OFFSET_COLOR = sizeof(vertex2d);
    static const uint8_t OFFSET_TEXTURE =
        sizeof(vertex2d) + sizeof(color::rgba);
};

struct vertex3d_colored_textured : vertex3d
{
    vertex3d_colored_textured();
    vertex3d_colored_textured(vertex3d ver, color::rgba clr, vector2d uv);

    color::rgba rgba;
    vector2d    uv;

    static const uint8_t OFFSET_COLOR = sizeof(vertex3d);
    static const uint8_t OFFSET_TEXTURE =
        sizeof(vertex3d) + sizeof(color::rgba);
};

inline float dot(const vector3d& x, const vector3d& y)
{
    return x.x * y.x + x.y * y.y + x.z * y.z;
}

inline vector3d cross(const vector3d& x, const vector3d& y)
{
    return vector3d(
        x.y * y.z - y.y * x.z, x.z * y.x - y.z * x.x, x.x * y.y - y.x * x.y);
}

template <class T>
struct triangle
{
    triangle(){};
    triangle(T v0, T v1, T v2)
    {
        vertexes[0] = v0;
        vertexes[1] = v1;
        vertexes[2] = v2;
        calc_normal();
    }
    triangle(T v0, T v1, T v2, vector3d norm)
    {
        vertexes[0] = v0;
        vertexes[1] = v1;
        vertexes[2] = v2;
        normal      = norm;
    }

    void calc_normal()
    {
        normal = cross(vertexes[2].pos - vertexes[0].pos,
                       vertexes[1].pos - vertexes[0].pos)
                     .normalize();
    }

    T& operator[](const size_t index)
    {
        assert(index >= 3);
        return vertexes[index];
    }

    T*   data() { return vertexes.data(); }
    auto begin() { return vertexes.begin(); }
    auto end() { return vertexes.end(); }

    std::array<T, 3> vertexes;
    vector3d         normal;
};

struct membuff
{
    std::unique_ptr<char[]> ptr  = nullptr;
    size_t                  size = 0;
};

membuff* load_file_to_memory(const char* path);