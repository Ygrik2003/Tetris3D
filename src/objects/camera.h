#pragma once
#include "object.h"

class camera : public object
{
public:
    camera(float speed);

    void uniform_link(uniform& uni) override;

    void move(float dx, float dy, float dz);
    void move_forward(float distance);
    void move_backward(float distance);
    void move_left(float distance);
    void move_right(float distance);

    void set_move_forward(bool state);
    void set_move_backward(bool state);
    void set_move_left(bool state);
    void set_move_right(bool state);

    void update();

    void add_rotate(float dalpha, float dbeta, float dgamma);

    float get_x() const { return dx; }
    float get_y() const { return dy; }
    float get_z() const { return dz; }

private:
    float speed;

    bool move_forward_state  = false;
    bool move_backward_state = false;
    bool move_left_state     = false;
    bool move_right_state    = false;
};