#pragma once

struct config
{
    const char* app_name               = "Tetris 3D";
    const char* shader_vertex          = "res/shaders/shader.vert";
    const char* shader_fragment        = "res/shaders/shader.frag";
    const char* shader_vertex_imgui    = "res/shaders/shader_imgui.vert";
    const char* shader_fragment_imgui  = "res/shaders/shader_imgui.frag";
    const char* texture_block          = "res/textures/texture_block.png";
    const char* texture_board          = "res/textures/texture_board.png";
    const char* sound_background_music = "res/sounds/background_music.wav";
    const char* sound_collision        = "res/sounds/collision.wav";
    const char* model_board            = "res/models/board.obj";
    const char* model_cube             = "res/models/cube.obj";

    double width          = 1920 - 100;
    double height         = 1080 - 100;
    bool   is_full_screen = true;

    float camera_speed_rotate = 1. / 100.;
    float camera_speed        = 0.05;
};