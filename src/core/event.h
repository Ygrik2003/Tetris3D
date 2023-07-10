#pragma once

#include <cstdint>

struct event
{
    struct event_mouse
    {
        uint8_t left_clicked : 1;
        uint8_t left_released : 1;
        uint8_t right_clicked : 1;
        uint8_t right_released : 1;
        uint8_t center_clicked : 1;
        uint8_t center_released : 1;
        uint8_t back_clicked : 1;
        uint8_t back_released : 1;
        uint8_t next_clicked : 1;
        uint8_t next_released : 1;
    } mouse;

    struct event_motion
    {
        float x;
        float y;
        float x_rel;
        float y_rel;
    } motion;

    struct event_keyboard
    {
        uint8_t w_clicked : 1;
        uint8_t w_released : 1;
        uint8_t s_clicked : 1;
        uint8_t s_released : 1;
        uint8_t a_clicked : 1;
        uint8_t a_released : 1;
        uint8_t d_clicked : 1;
        uint8_t d_released : 1;
        uint8_t space_clicked : 1;
        uint8_t space_released : 1;
        uint8_t left_clicked : 1;
        uint8_t left_released : 1;
        uint8_t right_clicked : 1;
        uint8_t right_released : 1;
        uint8_t up_clicked : 1;
        uint8_t up_released : 1;
        uint8_t down_clicked : 1;
        uint8_t down_released : 1;
    } keyboard;

    struct event_action
    {
        uint8_t quit : 1;
        uint8_t resize : 1;
        uint8_t skip : 1;

    } action;

    void clear()
    {
        mouse    = event_mouse{};
        motion   = event_motion{};
        keyboard = event_keyboard{};
        action   = event_action{};
    }
};
