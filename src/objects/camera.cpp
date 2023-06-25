#include "camera.h"

camera::camera(float speed)
{
    set_rotate(0, 0, 0);
    set_translate(0, 0, 0);
    set_scale(1, 1, 1);

    this->speed = speed;
}

void camera::uniform_link(uniform& uni)
{
    uni.rotate_alpha_camera = &alpha;
    uni.rotate_beta_camera  = &beta;
    uni.rotate_gamma_camera = &gamma;

    uni.translate_x_camera = &dx;
    uni.translate_y_camera = &dy;
    uni.translate_z_camera = &dz;
}

void camera::move(float dx, float dy, float dz)
{
    this->dx += dx;
    this->dy += dy;
    this->dz += dz;
}

void camera::move_forward(float distance)
{
    move(-distance * std::cos(gamma) * std::cos(beta - M_PI / 2),
         distance * std::sin(gamma),
         distance * std::cos(gamma) * std::sin(beta - M_PI / 2));
}

void camera::move_backward(float distance)
{
    move(distance * std::cos(gamma) * std::cos(beta - M_PI / 2),
         -distance * std::sin(gamma),
         -distance * std::cos(gamma) * std::sin(beta - M_PI / 2));
}

void camera::move_left(float distance)
{
    move(distance * std::cos(beta), 0, -distance * std::sin(beta));
}

void camera::move_right(float distance)
{
    move(-distance * std::cos(beta), 0, distance * std::sin(beta));
}

void camera::set_move_forward(bool state)
{
    move_forward_state = state;
}
void camera::set_move_backward(bool state)
{
    move_backward_state = state;
}
void camera::set_move_left(bool state)
{
    move_left_state = state;
}
void camera::set_move_right(bool state)
{
    move_right_state = state;
}

void camera::update()
{
    // clang-format off
    if (move_forward_state)  move_forward(speed);
    if (move_backward_state) move_backward(speed);
    if (move_left_state)     move_left(speed);
    if (move_right_state)    move_right(speed);
    // clang-format on
}

void camera::add_rotate(float dalpha, float dbeta, float dgamma)
{
    alpha += dalpha;
    beta += dbeta;
    gamma += dgamma;
    if (gamma > M_PI / 2)
        gamma = M_PI / 2;
    if (gamma < -M_PI / 2)
        gamma = -M_PI / 2;
}
