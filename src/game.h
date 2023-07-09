#pragma once
#include "core/event.h"
#include "core/types.h"
#include "engine/engine_opengl.h"
#include "objects/camera.h"

#include <array>

using namespace std::chrono;

constexpr uint32_t fps = 60;

enum class direction
{
    left,
    forward,
    right,
    backward,
    up,
    down,
    last
};

enum class axis
{
    x, 
    y, 
    z
};

class cell
{
public:
    struct position
    {
        position()
            : x(0)
            , y(0)
            , z(0)
        {
        }
        position(int _x, int _y, int _z)
            : x(_x)
            , y(_y)
            , z(_z)
        {
        }
        position operator+(const position& r)
        {
            position ret;
            ret.x = x + r.x;
            ret.y = y + r.y;
            ret.z = z + r.z;
            return ret;
        }
        position operator-(const position& r)
        {
            position ret;
            ret.x = x - r.x;
            ret.y = y - r.y;
            ret.z = z - r.z;
            return ret;
        }
        bool operator==(const position& r)
        {
            return (x == r.x) && (y == r.y) && (z == r.z);
        }

        int x;
        int y;
        int z;
    };

    cell(position _pos, uint8_t _texture_index)
        : pos(_pos)
        , texture_index(_texture_index)
    {
    }

    cell* get_cell_near(direction dir)
    {
        return neighboors[static_cast<int>(dir)];
    }
    void set_cell_near(direction dir, cell* c)
    {
        neighboors[static_cast<int>(dir)] = c;
    }
    uint8_t get_texture_index() { return texture_index; }
    bool        get_moving() { return is_moving; }
    void        set_moving(bool state) { is_moving = state; }
    position    get_position() { return pos; }
    void        set_position(position _pos) { pos = _pos; }

private:
    position             pos;
    uint8_t              texture_index;
    std::array<cell*, 6> neighboors{ nullptr };
    bool                 is_moving = true;
};

class primitive
{
public:
    primitive(cell* _root)
        : root(_root)
    {
    }
    cell* get_root() { return root; }

private:
    cell*     root;
    direction rotate{ 0 };
    bool      is_active = true;
};

constexpr int cells_max    = 8;
constexpr int cells_max_z  = 14;
constexpr int cells_z_lose = 10;

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
    void draw_restart_menu();
    void draw_ui();
    void render_scene();

    void start_game();
    void lose_game();
    void add_primitive();

    bool move_active_cells(direction dir);
    bool rotate_around(axis ax);
    bool check_moving(cell* c, direction dir, std::vector<cell*>& visited);
    void find_near(cell* c);
    void collision();
    void check_layer();

    config cfg;
    size_t score = 0;
    float  delay = 1; // Seconds

    uniform              uniforms;
    figure*              figure_board;
    figure*              figure_cube;
    std::vector<figure*> figures;

    primitive*         active_primitive = nullptr;
    std::vector<cell*> cells;

    shader*               shader_scene  = nullptr;
    texture*              texture_board = nullptr;
    std::vector<texture*> textures_block;

    double camera_angle = -M_PI / 2;
    double view_height  = 1.;
    double min_view_height  = 1.;
    double max_view_height  = 1.6;

    struct flags
    {
        uint8_t is_started : 1;
        uint8_t is_restart : 1;
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