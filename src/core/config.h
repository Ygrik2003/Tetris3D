#pragma once

struct config
{
    const char* app_name               = "Tetris 3D";
    const char* shader_vertex          = "res/shaders/shader.vert";
    const char* shader_fragment        = "res/shaders/shader.frag";
    const char* shader_vertex_imgui    = "res/shaders/shader_imgui.vert";
    const char* shader_fragment_imgui  = "res/shaders/shader_imgui.frag";
    const char* texture_block_1        = "res/textures/texture_block_1.png";
    const char* texture_block_2        = "res/textures/texture_block_2.png";
    const char* texture_block_3        = "res/textures/texture_block_3.png";
    const char* texture_block_4        = "res/textures/texture_block_4.png";
    const char* texture_board          = "res/textures/texture_board.png";
    const char* texture_button_control = "res/textures/texture_board.png";
    const char* sound_background_music = "res/sounds/background_music.wav";
    const char* sound_collision        = "res/sounds/collision.wav";
    const char* model_board            = "res/models/board.obj";
    const char* model_cube             = "res/models/cube.obj";

    double width          = 1600 - 100;
    double height         = 900 - 100;
    bool   is_full_screen = false;

    float camera_speed_rotate = 1. / 100.;
    float camera_speed        = 0.05;
};