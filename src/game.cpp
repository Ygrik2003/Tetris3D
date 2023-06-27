#include "game.h"
#include "objects/model.h"

game_tetris::game_tetris()
{
    state.is_started = 1;
    state.is_quit    = 0;
    state.is_rotated = 0;
    state.is_moving  = 0;

    figure_board = model(cfg.model_board).get_figure();
    figure_cube  = model(cfg.model_cube).get_figure();

    buffer_z.resize(cells_max * cells_max);

    columns.resize(cells_max * cells_max * cells_max_z);
    for (column& col : columns)
    {
        col = column{ 0 };
    }
}

int game_tetris::initialize(config cfg)
{

    this->cfg = cfg;

    uniforms.width  = cfg.width;
    uniforms.height = cfg.height;

    cam = new camera(cfg.camera_speed);
    cam->uniform_link(uniforms);

    my_engine = new engine_opengl();

    my_engine->set_uniform(uniforms);
    if (!my_engine->initialize(this->cfg))
        return -1;

    shader_scene = new shader_opengl(cfg.shader_vertex, cfg.shader_fragment);
    my_engine->set_shader(shader_scene);

    texture_block = my_engine->load_texture(1, cfg.texture_block);
    texture_board = my_engine->load_texture(2, cfg.texture_board);

    add_figure(figure_board, texture_board);
    add_primitive();
    my_engine->play_sound(cfg.sound_background_music, true);

    return 1;
};

bool game_tetris::event_listener(event& e)
{
    e.clear();
    if (my_engine->event_keyboard(e))
    {
        // Always listen buttons
        if (e.action.quit)
        {
            return false;
        }
        // Only game buttons
        if (state.is_started)
        {
            if (e.mouse.left_cliked)
            {
                state.is_rotated = true;
                state.is_moving  = true;
            }
            if (e.mouse.left_released)
            {
                state.is_rotated = false;
                state.is_moving  = false;
            }
            if (e.motion.x && state.is_rotated)
            {
                camera_angle += e.motion.x * M_PI / 300;
            }
            if (e.motion.y && state.is_rotated)
            {
                view_height += e.motion.y / 50;
                if (view_height < 1.)
                    view_height = 1.;
                if (view_height > 1.6)
                    view_height = 1.6;
            }

            if (e.keyboard.w_clicked)
            {
                move_cell(direction::forward);
            }
            if (e.keyboard.s_clicked)
            {
                move_cell(direction::backward);
            }
            if (e.keyboard.a_clicked)
            {
                move_cell(direction::left);
            }
            if (e.keyboard.d_clicked)
            {
                move_cell(direction::right);
            }

            // Free Camera
            // if (e.motion.x || e.motion.y)
            // {
            //     cam->add_rotate(0,
            //                     cfg.camera_speed_rotate * e.motion.x,
            //                     cfg.camera_speed_rotate * e.motion.y);
            // }
            // if (e.keyboard.w_clicked)
            //     cam->set_move_forward(true);
            // if (e.keyboard.w_released)
            //     cam->set_move_forward(false);
            // if (e.keyboard.s_clicked)
            //     cam->set_move_backward(true);
            // if (e.keyboard.s_released)
            //     cam->set_move_backward(false);
            // if (e.keyboard.a_clicked)
            //     cam->set_move_left(true);
            // if (e.keyboard.a_released)
            //     cam->set_move_left(false);
            // if (e.keyboard.d_clicked)
            //     cam->set_move_right(true);
            // if (e.keyboard.d_released)
            //     cam->set_move_right(false);
        }
    }
    return true;
};

void game_tetris::update()
{
    static std::chrono::steady_clock timer;
    static auto                      last_time_update = timer.now();

    cam->update();
    cam->set_rotate(
        0, M_PI / 2 + camera_angle, M_PI / 2 - atan(1. / sqrt(view_height)));
    cam->set_translate(sqrt(view_height) * std::cos(camera_angle),
                       -view_height,
                       -sqrt(view_height) * std::sin(camera_angle));

    if ((timer.now() - last_time_update).count() < delay * 1e9)
        return;
    last_time_update = timer.now();

    bool is_stop_falling = false;

    for (int x = 0; x < cells_max; x++)
    {
        for (int y = 0; y < cells_max; y++)
        {
            if (get_cell_color(x, y, get_column_z(x, y)))
            {
                update_buffer_z();
                check_layer();
                add_primitive();
                my_engine->play_sound(cfg.sound_collision, false);
                is_stop_falling = true;
                break;
            }
            get_column(x, y).move_down_over(get_column_z(x, y));
        }
        if (is_stop_falling)
            break;
    }
}

void game_tetris::render()
{
    ImGui::NewFrame();
    if (!state.is_started)
    {
        draw_menu();
    }
    else
    {
        shader_scene->use();
        render_scene();
        draw_ui();
    }
    ImGui::Render();
    my_engine->swap_buffers();
};
void game_tetris::add_figure(figure* fig, texture* tex)
{
    fig->set_texture(tex);
    figures.push_back(fig);
}
void game_tetris::draw_menu()
{
    static const int window_width  = 0.2 * cfg.width;
    static const int window_height = 0.2 * cfg.height;
    static const int window_x      = (cfg.width - window_width) / 2;
    static const int window_y      = (cfg.height - window_height) / 2;

    ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
    ImGui::SetNextWindowPos(ImVec2(window_x, window_y));

    ImGui::Begin("Menu",
                 0,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    if (ImGui::Button("Start", ImVec2(0.1 * cfg.width, 0.05 * cfg.height)))
    {
        start_game();
    }
    if (ImGui::Button("Settings", ImVec2(0.15 * cfg.width, 0.05 * cfg.height)))
    {
    }
    if (ImGui::Button("Quit", ImVec2(0.09 * cfg.width, 0.05 * cfg.height)))
    {
    }

    ImGui::End();
}
void game_tetris::draw_ui()
{
    static const int window_score_width  = 0.1 * cfg.width;
    static const int window_score_height = 0.03 * cfg.height;
    static const int window_score_x      = 10;
    static const int window_score_y      = 10;

    static const int window_control_width  = 0.30 * cfg.width;
    static const int window_control_height = 0.35 * cfg.height;
    static const int window_control_x      = 20;
    static const int window_control_y      = 0.6 * cfg.height;

    static const int window_rotate_width  = 0.2 * cfg.width;
    static const int window_rotate_height = 0.95 * cfg.height;
    static const int window_rotate_x      = 0.8 * cfg.width - 10;
    static const int window_rotate_y      = 10;

    static const ImVec2 button_rotate_size =
        ImVec2(window_rotate_width - 20, window_rotate_height / 3 - 10);
    static const ImVec2 button_control_size =
        ImVec2(window_control_width / 3 - 10, window_control_height / 2 - 10);

    ImGui::SetNextWindowSize(ImVec2(window_score_width, window_score_height));
    ImGui::SetNextWindowPos(ImVec2(window_score_x, window_score_y));

    ImGui::Begin("State",
                 0,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("Score: %zu", score);

    ImGui::End();

    ImGui::SetNextWindowSize(
        ImVec2(window_control_width, window_control_height));
    ImGui::SetNextWindowPos(ImVec2(window_control_x, window_control_y));

    ImGui::Begin("Controller",
                 0,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoBackground);

    ImGui::SetCursorPos(ImVec2(button_control_size.x + 15, 5));
    if (ImGui::Button("Forvard", button_control_size))
    {
        move_cell(direction::forward);
    }
    ImGui::SetCursorPos(ImVec2(7, button_control_size.y + 15));
    if (ImGui::Button("Backward", button_control_size))
    {
        move_cell(direction::backward);
    }
    ImGui::SameLine();
    if (ImGui::Button("Left", button_control_size))
    {
        move_cell(direction::left);
    }
    ImGui::SameLine();
    if (ImGui::Button("Right", button_control_size))
    {
        move_cell(direction::right);
    }

    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(window_rotate_width, window_rotate_height));
    ImGui::SetNextWindowPos(ImVec2(window_rotate_x, window_rotate_y));

    ImGui::Begin("Rotate",
                 0,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoBackground);

    if (ImGui::Button("Rotate X axis", button_rotate_size))
    {
    }
    if (ImGui::Button("Rotate Y axis", button_rotate_size))
    {
    }
    if (ImGui::Button("Rotate Z axis", button_rotate_size))
    {
    }
    ImGui::End();
}
void game_tetris::render_scene()
{

    for (figure* fig : figures)
    {
        fig->uniform_link(uniforms);

        vertex_buffer<vertex3d_textured>* vertex_buff = new vertex_buffer(
            fig->get_vertexes().data(), fig->get_vertexes().size());
        index_buffer* index_buff = new index_buffer(fig->get_indexes().data(),
                                                    fig->get_indexes().size());
        my_engine->reload_uniform();
        my_engine->render_triangles(
            vertex_buff, index_buff, fig->get_texture(), 0, index_buff->size());

        delete vertex_buff;
        delete index_buff;
    }
    vertex_buffer<vertex3d_textured>* vertex_buff = new vertex_buffer(
        figure_cube->get_vertexes().data(), figure_cube->get_vertexes().size());
    index_buffer* index_buff = new index_buffer(
        figure_cube->get_indexes().data(), figure_cube->get_indexes().size());

    for (int x = 0; x < cells_max; x++)
        for (int y = 0; y < cells_max; y++)
        {
            for (uint16_t z = 0; z < column_count_cells; z++)
            {
                if (!get_column(x, y).get_z(z))
                    continue;
                figure_cube->set_translate(
                    vector3d(-1. / 2. + (x + 0.5) / cells_max,
                             (z + 0.5) / cells_max,
                             -1. / 2. + (y + 0.5) / cells_max));
                figure_cube->set_texture(texture_block);
                figure_cube->uniform_link(uniforms);

                my_engine->reload_uniform();
                my_engine->render_triangles(vertex_buff,
                                            index_buff,
                                            figure_cube->get_texture(),
                                            0,
                                            index_buff->size());
            }
        }

    delete vertex_buff;
    delete index_buff;
}
void game_tetris::start_game()
{
    state.is_started = ~state.is_started;
}

void game_tetris::add_primitive()
{
    static float x = 0;
    static float y = 0;

    set_cell_color(x, y, cells_max_z, 1);
    // set_cell_color(x + 1, y, cells_max_z, 1);
    // set_cell_color(x + 2, y, cells_max_z, 1);
    // set_cell_color(x + 1, y, cells_max_z - 1, 1);

    x++;
    if (x == cells_max)
    {
        x = 0;
        y += 0.5;
        if (y == cells_max)
            y = 0;
    }
}

void game_tetris::set_cell_color(uint8_t x, uint8_t y, uint8_t z, uint8_t clr)
{
    columns[y * cells_max + x] |= static_cast<uint32_t>(clr)
                                  << (column_bit_for_color * z);
}

uint8_t game_tetris::get_cell_color(uint8_t x, uint8_t y, uint8_t z)
{
    return get_column(x, y).get_z(z);
}

column& game_tetris::get_column(uint8_t x, uint8_t y)
{
    if (x < 0 || y < 0 || x >= cells_max || y >= cells_max)
        return column_wall;
    return columns[y * cells_max + x];
}

uint8_t game_tetris::get_column_z(uint8_t x, uint8_t y)
{
    if (x < 0 || y < 0 || x >= cells_max || y >= cells_max)
        return -1;
    return buffer_z[y * cells_max + x];
}

void game_tetris::set_column_z(uint8_t x, uint8_t y, uint8_t byte)
{
    buffer_z[y * cells_max + x] = byte;
}

void game_tetris::update_buffer_z()
{
    for (int x = 0; x < cells_max; x++)
    {
        for (int y = 0; y < cells_max; y++)
        {
            for (int z = get_column_z(x, y); z < cells_max_z; z++)
            {
                if (get_cell_color(x, y, z))
                    set_column_z(x, y, z + 1);
            }
        }
    }
}

positions game_tetris::get_bound_object(direction dir, const positions& object)
{
    uint8_t   bound;
    positions out;
    switch (dir)
    {
        case direction::left:
            bound = std::min_element(object.begin(),
                                     object.end(),
                                     [&](const std::pair<uint8_t, uint8_t>& p1,
                                         const std::pair<uint8_t, uint8_t>& p2)
                                     { return p1.first < p2.first; })
                        ->first;
            for (auto point : object)
            {
                if (point.first == bound)
                {
                    out.push_back(point);
                }
            }
            break;
        case direction::right:
            bound = std::max_element(object.begin(),
                                     object.end(),
                                     [&](const std::pair<uint8_t, uint8_t>& p1,
                                         const std::pair<uint8_t, uint8_t>& p2)
                                     { return p1.first < p2.first; })
                        ->first;
            for (auto point : object)
            {
                if (point.first == bound)
                {
                    out.push_back(point);
                }
            }
            break;
        case direction::forward:
            bound = std::max_element(object.begin(),
                                     object.end(),
                                     [&](const std::pair<uint8_t, uint8_t>& p1,
                                         const std::pair<uint8_t, uint8_t>& p2)
                                     { return p1.second < p2.second; })
                        ->second;
            for (auto point : object)
            {
                if (point.second == bound)
                {
                    out.push_back(point);
                }
            }
            break;
        case direction::backward:
            bound = std::min_element(object.begin(),
                                     object.end(),
                                     [&](const std::pair<uint8_t, uint8_t>& p1,
                                         const std::pair<uint8_t, uint8_t>& p2)
                                     { return p1.second < p2.second; })
                        ->second;
            for (auto point : object)
            {
                if (point.second == bound)
                {
                    out.push_back(point);
                }
            }
            break;
    }

    return out;
}

void game_tetris::move_cell(direction dir)
{
    dir = static_cast<direction>(
        (static_cast<int>(dir) +
         static_cast<int>(M_PI / 2 + camera_angle / (M_PI / 2))) %
        4);

    positions object;
    int8_t    shift_x = 0, shift_y = 0;

    for (uint8_t x = 0; x < cells_max; x++)
    {
        for (uint8_t y = 0; y < cells_max; y++)
        {
            for (uint8_t z = get_column_z(x, y); z < cells_max_z; z++)
            {
                if (get_cell_color(x, y, z))
                {
                    object.push_back({ x, y });
                }
            }
        }
    }
    if (object.empty())
        return;

    switch (dir)
    {
        case direction::left:
            shift_x = -1;
            std::sort(object.begin(),
                      object.end(),
                      [&](const std::pair<uint8_t, uint8_t>& p1,
                          const std::pair<uint8_t, uint8_t>& p2)
                      { return p1.first > p2.first; });
            break;
        case direction::right:
            shift_x = 1;
            std::sort(object.begin(),
                      object.end(),
                      [&](const std::pair<uint8_t, uint8_t>& p1,
                          const std::pair<uint8_t, uint8_t>& p2)
                      { return p1.first < p2.first; });
            break;
        case direction::forward:
            shift_y = 1;
            std::sort(object.begin(),
                      object.end(),
                      [&](const std::pair<uint8_t, uint8_t>& p1,
                          const std::pair<uint8_t, uint8_t>& p2)
                      { return p1.second < p2.second; });
            break;
        case direction::backward:
            shift_y = -1;
            std::sort(object.begin(),
                      object.end(),
                      [&](const std::pair<uint8_t, uint8_t>& p1,
                          const std::pair<uint8_t, uint8_t>& p2)
                      { return p1.second > p2.second; });
            break;
    }

    positions bound_object = get_bound_object(dir, object);
    for (auto [x, y] : bound_object)
    {
        column from_column = get_column(x, y);
        column to_column   = get_column(x + shift_x, y + shift_y);

        from_column &= ~((1 << column_bit_for_color * get_column_z(x, y)) - 1);
        to_column &= ((1 << column_bit_for_color *
                                get_column_z(x + shift_x, y + shift_y)) -
                      1);

        if (~(~from_column | ~to_column))
            return;
    }

    for (auto [x, y] : object)
    {
        column& from_column = get_column(x, y);
        column& to_column   = get_column(x + shift_x, y + shift_y);

        uint32_t mask_to = (1 << column_bit_for_color *
                                     get_column_z(x + shift_x, y + shift_y)) -
                           1;
        uint32_t mask_from =
            (1 << column_bit_for_color * get_column_z(x, y)) - 1;

        to_column |= from_column & ~std::max(mask_to, mask_from);
        from_column &= mask_from;
    }
}

void game_tetris::check_layer()
{
    bool is_full_layer;
    for (int z = 0; z < cells_max_z; z++)
    {
        is_full_layer = true;
        for (int x = 0; x < cells_max; x++)
        {
            for (int y = 0; y < cells_max; y++)
            {
                if (!get_cell_color(x, y, z))
                    is_full_layer = false;
            }
        }
        if (!is_full_layer)
            continue;
        score++;
        for (int x = 0; x < cells_max; x++)
        {
            for (int y = 0; y < cells_max; y++)
            {
                get_column(x, y).move_down_over(z);
                set_column_z(x, y, get_column_z(x, y) - 1);
            }
        }
    }
}
