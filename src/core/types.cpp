#include "types.h"
#include "SDL3/SDL_rwops.h"
#include <string>

color::rgba::rgba() {}
color::rgba::rgba(float r, float g, float b, float a)
{
    set_r(r);
    set_g(g);
    set_b(b);
    set_a(a);
}

vector3d::vector3d() {}
vector3d::vector3d(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
};
vector3d vector3d::operator+(const vector3d& right)
{
    return vector3d(this->x + right.x, this->y + right.y, this->z + right.z);
}
vector3d vector3d::operator+=(const vector3d& right)
{
    this->x += right.x;
    this->y += right.y;
    this->z += right.z;
    return *this;
}
vector3d vector3d::operator-(const vector3d& right)
{
    return vector3d(this->x - right.x, this->y - right.y, this->z - right.z);
}
vector3d vector3d::operator*(const float& right)
{
    return vector3d(this->x * right, this->y * right, this->z * right);
}

vector3d vector3d::operator/(const float& right)
{
    return vector3d(this->x / right, this->y / right, this->z / right);
}

vector3d operator*(const float& left, const vector3d& right)
{
    return vector3d(right.x * left, right.y * left, right.z * left);
}

vector3d& vector3d::operator=(const vector3d& right)
{
    this->x = right.x;
    this->y = right.y;
    this->z = right.z;
    return *this;
}
float vector3d::length() const
{
    return std::pow(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2), 1. / 2.);
}
vector3d& vector3d::normalize()
{
    float len = length();
    x /= len;
    y /= len;
    z /= len;
    return *this;
}

vector2d::vector2d() {}
vector2d::vector2d(float x, float y)
{
    this->x = x;
    this->y = y;
};
vector2d vector2d::operator+(const vector2d& right)
{
    return vector2d(this->x + right.x, this->y + right.y);
}
vector2d vector2d::operator+=(const vector2d& right)
{
    this->x += right.x;
    this->y += right.y;
    return *this;
}
vector2d vector2d::operator-(const vector2d& right)
{
    return vector2d(this->x - right.x, this->y - right.y);
}
vector2d& vector2d::operator=(const vector2d& right)
{
    this->x = right.x;
    this->y = right.y;
    return *this;
}
float vector2d::length() const
{
    return std::pow(std::pow(x, 2) + std::pow(y, 2), 1. / 2.);
}
vector2d& vector2d::normalize()
{
    float len = length();
    x /= len;
    y /= len;
    return *this;
}

vector4d::vector4d() {}
vector4d::vector4d(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
};
vector4d vector4d::operator+(const vector4d& right)
{
    return vector4d(this->x + right.x,
                    this->y + right.y,
                    this->z + right.z,
                    this->w + right.w);
}
vector4d vector4d::operator+=(const vector4d& right)
{
    this->x += right.x;
    this->y += right.y;
    this->z += right.z;
    this->w += right.w;
    return *this;
}
vector4d vector4d::operator-(const vector4d& right)
{
    return vector4d(this->x - right.x,
                    this->y - right.y,
                    this->z - right.z,
                    this->w - right.w);
}
vector4d& vector4d::operator=(const vector4d& right)
{
    this->x = right.x;
    this->y = right.y;
    this->w = right.z;
    this->z = right.w;
    return *this;
}
float vector4d::length() const
{
    return std::pow(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2) +
                        std::pow(w, 2),
                    1. / 2.);
}
vector4d& vector4d::normalize()
{
    float len = length();
    x /= len;
    y /= len;
    z /= len;
    w /= len;
    return *this;
}

vertex2d::vertex2d() {}
vertex2d::vertex2d(float x, float y)
{
    this->pos.x = x;
    this->pos.y = y;
}

vertex3d::vertex3d() {}
vertex3d::vertex3d(float x, float y, float z)
{
    this->pos.x = x;
    this->pos.y = y;
    this->pos.z = z;
}

vertex2d_colored::vertex2d_colored() {}
vertex2d_colored::vertex2d_colored(vertex2d ver, color::rgba color)
{
    this->pos  = ver.pos;
    this->rgba = color;
}
vertex3d_colored::vertex3d_colored() {}
vertex3d_colored::vertex3d_colored(vertex3d ver, color::rgba color)
{
    this->pos  = ver.pos;
    this->rgba = color;
}

vertex2d_textured::vertex2d_textured() {}
vertex2d_textured::vertex2d_textured(vertex2d ver, vector2d uv)
{
    this->pos = ver.pos;
    this->uv  = uv;
}

vertex3d_textured::vertex3d_textured() {}
vertex3d_textured::vertex3d_textured(vertex3d ver, vector2d uv)
{
    this->pos = ver.pos;
    this->uv  = uv;
}

vertex2d_colored_textured::vertex2d_colored_textured() {}
vertex2d_colored_textured::vertex2d_colored_textured(vertex2d    ver,
                                                     color::rgba clr,
                                                     vector2d    uv)
{
    this->pos  = ver.pos;
    this->rgba = clr;
    this->uv   = uv;
}

vertex3d_colored_textured::vertex3d_colored_textured() {}
vertex3d_colored_textured::vertex3d_colored_textured(vertex3d    ver,
                                                     color::rgba clr,
                                                     vector2d    uv)
{
    this->pos  = ver.pos;
    this->rgba = clr;
    this->uv   = uv;
}

membuff* load_file_to_memory(const char* path)
{
    SDL_RWops* file = SDL_RWFromFile(path, "rb");
    if (file == nullptr)
    {
        throw std::runtime_error("can't load file: " + std::string(path));
    }

    Sint64 file_size = file->size(file);
    if (file_size == -1)
    {
        throw std::runtime_error("can't determine size of file: " +
                                 std::string(path));
    }
    const size_t            size = static_cast<size_t>(file_size);
    std::unique_ptr<char[]> mem  = std::make_unique<char[]>(size);

    const size_t num_readed_objects = file->read(file, mem.get(), size);
    if (num_readed_objects != size)
    {
        throw std::runtime_error("can't read all content from file: " +
                                 std::string(path));
    }

    if (file->close(file) != 0)
    {
        throw std::runtime_error("failed close file: " + std::string(path));
    }
    mem[size] = 0;
    return new membuff{ std::move(mem), size };
}
