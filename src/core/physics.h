#include "types.h"
#include <chrono>

using time_point = std::chrono::time_point<std::chrono::steady_clock,
                                           std::chrono::nanoseconds>;

class physics
{
public:
    physics(vector3d start_pos);

    void set_speed(vector3d speed);
    void set_speed(float x, float y, float z);
    // vector3d get_speed();

    void set_acceleration(vector3d acceleration);
    void set_acceleration(float x, float y, float z);
    // vector3d get_acceleration();

    void     set_position(vector3d pos);
    void     set_position(float x, float y, float z);
    vector3d get_position();

    vector3d get_velocity();

    void set_physics_state(bool state);

private:
    void calc_past_time();

    bool                      state = false;
    std::chrono::steady_clock timer;
    time_point                start_time;
    float                     past_time; // seconds

    vector3d start_position;
    vector3d speed;
    vector3d acceleration;
};