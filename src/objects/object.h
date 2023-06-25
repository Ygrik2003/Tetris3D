#pragma once
#include "core/types.h"

#include <cstdint>
#include <vector>

class object
{
public:
    virtual void set_rotate(float alpha, float beta, float gamma);
    virtual void set_translate(float dx, float dy, float dz);
    virtual void set_translate(vector3d pos);
    virtual void set_scale(float scale_x, float scale_y, float scale_z);
    virtual void uniform_link(uniform& uni){};

protected:
    float alpha = 0.f;
    float beta  = 0.f;
    float gamma = 0.f;

    float dx = 0.f;
    float dy = 0.f;
    float dz = 0.f;

    float scale_x = 0.f;
    float scale_y = 0.f;
    float scale_z = 0.f;
};