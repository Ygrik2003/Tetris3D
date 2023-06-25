#pragma once

struct config
{
    const char* app_name = "Tetris3D";
    const char* shader_vertex;
    const char* shader_fragment;
    const char* texture_block;
    const char* texture_board;
    const char* texture_checker_white;
    const char* texture_checker_black;
    double      width         = 1920 - 100;
    double      height        = 1080 - 100;
    bool        is_full_sreen = false;

    float camera_speed_rotate = 1. / 100.;
    float camera_speed        = 0.05;
};