#pragma once
#include "core/event.h"
#include "core/types.h"
#include "engine/engine_opengl.h"
#include "objects/camera.h"

using namespace std::chrono;
using positions = std::vector<std::pair<uint8_t, uint8_t>>;

constexpr uint32_t fps = 60;

enum class direction
{ 
    left,
    forward,
    right,
    backward,
};

constexpr size_t cells_max    = 8;
constexpr size_t cells_max_z  = 14;
constexpr size_t cells_z_lose = 10;

class game
{
public:
    virtual ~game()                     = default;
    virtual int  initialize(config)     = 0;
    virtual bool event_listener(event&) = 0;
    virtual void update()               = 0;
    virtual void render()               = 0;
    
protected:
    engine* my_engine = nullptr;
    camera* cam       = nullptr;
};

class game_tetris : public game
{
public:
    game_tetris();

    int  initialize(config) override;
    bool event_listener(event&) override;
    void update() override;
    void render() override;

    void add_figure(figure* fig, texture* texture);

private:
    void draw_menu();
    void draw_ui();
    void render_scene();

    void start_game();
    void add_primitive();

    bool move_active_cell(direction dir);
    void check_layer();


    config cfg;
    size_t score = 0;
    float  delay = 0.2; // Seconds

    uniform              uniforms;
    std::vector<figure*> figures;

    shader* shader_scene;

    figure* figure_board;
    figure* figure_cube;

    texture* texture_board = nullptr;
    texture* texture_block = nullptr;
    image    image_button_control;

    double camera_angle = -M_PI / 2;
    double view_height  = 1.;
    double theta        = 0;

    struct flags
    {
        uint8_t is_started : 1;
        uint8_t is_quit : 1;
        uint8_t is_rotated : 1;
        uint8_t is_moving : 1;

    } state;

    int window_score_width;
    int window_score_height;
    int window_score_x;
    int window_score_y;
    int window_control_width;
    int window_control_height;
    int window_control_x;
    int window_control_y;
    int window_rotate_width;
    int window_rotate_height;
    int window_rotate_x;
    int window_rotate_y;
};