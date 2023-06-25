#include "object.h"

void object::set_rotate(float alpha, float beta, float gamma)
{
    this->alpha = alpha;
    this->beta  = beta;
    this->gamma = gamma;
}
void object::set_translate(float dx, float dy, float dz)
{
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
}

void object::set_translate(vector3d pos)
{
    this->dx = pos.x;
    this->dy = pos.y;
    this->dz = pos.z;
}
void object::set_scale(float scale_x, float scale_y, float scale_z)
{
    this->scale_x = scale_x;
    this->scale_y = scale_y;
    this->scale_z = scale_z;
}