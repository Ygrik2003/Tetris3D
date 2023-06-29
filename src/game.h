#pragma once
#include "core/event.h"
#include "core/types.h"
#include "engine/engine_opengl.h"
#include "objects/camera.h"

using namespace std::chrono;
using positions = std::vector<std::pair<uint8_t, uint8_t>>;

constexpr uint32_t fps = 120;

enum class direction
{
    left,
    forward,
    right,
    backward,
};

constexpr uint32_t column_bit_for_color = 2;
constexpr uint32_t column_count_cells   = 16;

// clang-format off
/// @brief Bit field for definition each column in 3d tetris
/// The each bit field have 2 bits for set color of block in column
/// on height i. Position of block iterable each 2 bit. This struct is so
/// example for explanation this logic 
/// 00 - None 
/// 01 - red 
/// 10 - green 
/// 11 - blue
struct column
{
    // 2 smallest bit is color is lowest block, if color == 00, then block is absent
    uint32_t colors = 0;
    /// @brief Move all cells over selected position
    /// For example we have static cells at ground with height 1 and we needed omit all 
    /// cells start over selected position (in current case we can set position as 0 or 1 or 2, 
    /// in any way was selected height - 1). Func move_down_over(height - 1) transfer left column 
    /// to right column
    /// 0 | 0
    /// 1 | 0
    /// 1 | 1
    /// 0 | 1
    /// 0 | 0
    /// 1 | 1
    ///
    /// @param pos position, start from which cells was falling
    // clang-format on

    void     operator|=(const uint32_t& mask) { colors |= mask; }
    void     operator&=(const uint32_t& mask) { colors &= mask; }
    void     operator|=(const column& mask) { colors |= mask.colors; }
    void     operator&=(const column& mask) { colors &= mask.colors; }
    column   operator&(const uint32_t& mask) { return column{ colors & mask }; }
    uint32_t operator~() { return ~colors; }

    void move_down_over(uint8_t pos)
    {
        uint32_t mask = (1 << column_bit_for_color * pos) - 1;
        uint32_t result =
            ((colors & ~mask) >> column_bit_for_color) | (colors & mask);
        colors = result;
    }
    uint8_t get_z(uint32_t z)
    {
        uint8_t color_mask = (1 << column_bit_for_color) - 1;
        return (colors >> (column_bit_for_color * z)) & color_mask;
    }
};

constexpr size_t cells_max    = 8;
constexpr size_t cells_max_z  = 14; // no more 15
constexpr size_t cells_z_lose = 10;

class game
{
public:
    virtual ~game()                     = default;
    virtual int  initialize(config)     = 0;
    virtual bool event_listener(event&) = 0;
    virtual void update()               = 0;
    virtual void render()               = 0;
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

    void    set_cell_color(uint8_t x, uint8_t y, uint8_t z, uint8_t clr);
    uint8_t get_cell_color(uint8_t x, uint8_t y, uint8_t z);
    column& get_column(uint8_t x, uint8_t y);
    uint8_t get_column_z(uint8_t x, uint8_t y);
    void    set_column_z(uint8_t x, uint8_t y, uint8_t byte);

    void      update_buffer_z();
    positions get_bound_object(direction dir, const positions& object);
    void      move_cell(direction dir);
    void      check_layer();

    config cfg;
    size_t score = 0;
    float  delay = 0.2; // Seconds

    engine*              my_engine = nullptr;
    camera*              cam       = nullptr;
    uniform              uniforms;
    std::vector<figure*> figures;

    std::vector<column>  columns;
    column               column_wall{ static_cast<uint32_t>(-1) };
    std::vector<uint8_t> buffer_z;

    shader* shader_scene;
    shader* shader_temp;

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