#include "physics.h"

physics::physics(vector3d start_pos)
{
    this->start_position = start_pos;
}

void physics::set_speed(vector3d speed)
{
    this->speed = speed;
}

void physics::set_speed(float x, float y, float z)
{
    this->speed = vector3d(x, y, z);
}

void physics::set_acceleration(vector3d acceleration)
{
    this->acceleration = acceleration;
}

void physics::set_acceleration(float x, float y, float z)
{
    this->acceleration = vector3d(x, y, z);
}

void physics::set_position(vector3d pos)
{
    start_position = pos;
}
void physics::set_position(float x, float y, float z)
{
    start_position = vector3d(x, y, z);
}

vector3d physics::get_position()
{
    calc_past_time();
    return start_position + speed * past_time +
           acceleration * std::pow(past_time, 2) / 2;
}

vector3d physics::get_velocity()
{
    calc_past_time();
    return speed + acceleration * past_time;
}

void physics::set_physics_state(bool state)
{
    if (state == this->state)
        return;
    if (state)
    {
        start_time = timer.now();
    }
    else
    {
        start_position = get_position();
    }
    this->state = state;
}

void physics::calc_past_time()
{
    if (!state)
    {
        past_time = 0;
        return;
    }
    past_time = (timer.now() - start_time).count() / 1e9;
}
