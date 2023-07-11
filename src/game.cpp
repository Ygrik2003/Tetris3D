#include "game.h"
#include "objects/model.h"

game_tetris::game_tetris()
{
    state.is_started = 0;
    state.is_restart = 0;
    state.is_quit    = 0;
    state.is_rotated = 0;
    state.is_moving  = 0;

    figure_board = model(cfg.model_board).get_figure();
    figure_cube  = model(cfg.model_cube).get_figure();
}

int game_tetris::initialize(config _cfg)
{
    cfg = _cfg;

    cam = new camera(cfg.camera_speed);
    cam->uniform_link(uniforms);

    my_engine = new engine_opengl();

    my_engine->set_uniform(uniforms);
    if (!my_engine->initialize(cfg))
        return -1;

    uniforms.width  = cfg.width;
    uniforms.height = cfg.height;

    shader_scene = new shader_opengl(cfg.shader_vertex, cfg.shader_fragment);
    my_engine->set_shader(shader_scene);

    texture_board = my_engine->load_texture(1, cfg.texture_board);
    textures_block.push_back(my_engine->load_texture(2, cfg.texture_block_1));
    textures_block.push_back(my_engine->load_texture(3, cfg.texture_block_2));
    textures_block.push_back(my_engine->load_texture(4, cfg.texture_block_3));
    textures_block.push_back(my_engine->load_texture(5, cfg.texture_block_4));

    add_figure(figure_board, texture_board);
    my_engine->play_sound(cfg.sound_background_music, true);

    window_score_width  = 0.1f * cfg.width;
    window_score_height = 0.06f * cfg.height;
    window_score_x      = 10.f;
    window_score_y      = 10.f;

    window_control_width  = 0.30f * cfg.width;
    window_control_height = 0.35f * cfg.height;
    window_control_x      = 20.f;
    window_control_y      = 0.6f * cfg.height;
    window_rotate_width   = 0.2f * cfg.width;
    window_rotate_height  = 0.95f * cfg.height;
    window_rotate_x       = 0.8f * cfg.width - 10;
    window_rotate_y       = 10;

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
            if (e.mouse.left_clicked)
            {
                state.is_rotated = true;
                state.is_moving  = true;
            }
            if (e.mouse.left_released)
            {
                state.is_rotated = false;
                state.is_moving  = false;
            }
#ifdef __ANDROID__
            if (e.motion.x > window_control_width &&
                e.motion.x < window_rotate_x && !e.mouse.left_clicked)
#endif
            {
                if (e.motion.x_rel && state.is_rotated)
                {

                    if (abs(e.motion.x_rel) > cfg.max_camera_speed_swipe)
                        e.motion.x_rel = std::copysign(
                            cfg.max_camera_speed_swipe, e.motion.x_rel);

                    camera_angle += e.motion.x_rel * M_PI / 300;
                }
                if (e.motion.y_rel && state.is_rotated)
                {

                    if (abs(e.motion.y_rel) > cfg.max_camera_speed_swipe)
                        e.motion.y_rel = std::copysign(
                            cfg.max_camera_speed_swipe, e.motion.y_rel);

                    view_height += e.motion.y_rel / 50;
                    if (view_height < min_view_height)
                        view_height = min_view_height;
                    if (view_height > max_view_height)
                        view_height = max_view_height;
                }
            }

            if (e.keyboard.w_clicked)
            {
                move_active_cells(direction::forward);
            }
            if (e.keyboard.s_clicked)
            {
                move_active_cells(direction::backward);
            }
            if (e.keyboard.a_clicked)
            {
                move_active_cells(direction::left);
            }
            if (e.keyboard.d_clicked)
            {
                move_active_cells(direction::right);
            }
            if (e.keyboard.left_clicked)
            {
                rotate_around(axis::x);
            }
            if (e.keyboard.down_clicked)
            {
                rotate_around(axis::y);
            }
            if (e.keyboard.right_clicked)
            {
                rotate_around(axis::z);
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

    if ((timer.now() - last_time_update).count() < delay * 1e9 ||
        !state.is_started)
        return;
    last_time_update = timer.now();

    // check moving
    bool is_collision = false;
    for (cell* c : cells)
    {
        if (!c->get_moving())
            continue;
        if ((c->get_cell_near(direction::down) &&
             !c->get_cell_near(direction::down)->get_moving()) ||
            (c->get_position().z == 0 && c->get_moving()))
        {
            collision();
            return;
        }
        if ((c->get_cell_near(direction::down) != nullptr &&
             !c->get_cell_near(direction::down)->get_moving()) ||
            c->get_position().z == 0)
            return;
    }
    // if (is_collision)
    //{
    //     collision();
    //     return;
    // }
    //  moving
    for (cell* c : cells)
    {
        if (!c->get_moving())
            continue;
        cell::position cur_pos = c->get_position();
        if (c->get_moving())
            cur_pos.z--;
        c->set_position(cur_pos);
        find_near(c);
    }
}

void game_tetris::render()
{
    // ImGui::PushFont(font);
    ImGui::NewFrame();

    if (!state.is_started && !state.is_restart)
    {
        draw_menu();
    }
    else if (!state.is_started && state.is_restart)
    {
        draw_restart_menu();
    }
    else
    {
        shader_scene->use();
        render_scene();
        draw_ui();
    }
    ImGui::Render();
    // ImGui::PopFont();
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
    static ImVec2    button_size =
        ImVec2(window_width - 15, window_height / 2 - 15);

    ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
    ImGui::SetNextWindowPos(ImVec2(window_x, window_y));

    ImGui::Begin("Menu",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(4);

    if (ImGui::Button("Start", button_size))
    {
        start_game();
    }
    // if (ImGui::Button("Settings", ImVec2(0.15 * cfg.width, 0.05 *
    // cfg.height)))
    // {
    // }
    if (ImGui::Button("Quit", button_size))
    {
        state.is_quit = true;
    }

    ImGui::End();
}
void game_tetris::draw_restart_menu()
{
    static const float window_width  = 0.2f * cfg.width;
    static const float window_height = 0.2f * cfg.height;
    static const float window_x      = (cfg.width - window_width) / 2;
    static const float window_y      = (cfg.height - window_height) / 2;

    ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
    ImGui::SetNextWindowPos(ImVec2(window_x, window_y));

    ImGui::Begin("Restart",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(2.1);

    ImGui::LabelText("", "Score: %d", score);

    if (ImGui::Button("Restart", ImVec2(window_width - 15, 0.05 * cfg.height)))
    {
        start_game();
    }
    // if (ImGui::Button("Settings", ImVec2(0.15 * cfg.width, 0.05 *
    // cfg.height)))
    // {
    // }
    if (ImGui::Button("Quit", ImVec2(window_width - 15, 0.05 * cfg.height)))
    {
        state.is_quit = true;
    }

    ImGui::End();
}

void game_tetris::draw_ui()
{
    ImGui::SetNextWindowSize(ImVec2(window_score_width, window_score_height));
    ImGui::SetNextWindowPos(ImVec2(window_score_x, window_score_y));

    ImGui::Begin("State",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(2);

    ImGui::Text("Score: %zu", score);

    ImGui::End();

#ifdef __ANDROID__
    static const ImVec2 button_rotate_size =
        ImVec2(window_rotate_width - 20, window_rotate_height / 3 - 10);
    static const ImVec2 button_control_size =
        ImVec2(window_control_width / 3 - 10, window_control_height / 2 - 10);

    ImGui::SetNextWindowSize(
        ImVec2(window_control_width, window_control_height));
    ImGui::SetNextWindowPos(ImVec2(window_control_x, window_control_y));

    ImGui::Begin("Controller",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowFontScale(2);
    ImGui::SetCursorPos(ImVec2(button_control_size.x + 15, 5));

    if (ImGui::Button("Forvard", button_control_size))
    {
        move_active_cells(direction::forward);
    }
    ImGui::SetCursorPos(ImVec2(7, button_control_size.y + 15));
    if (ImGui::Button("Left", button_control_size))
    {
        move_active_cells(direction::left);
    }
    ImGui::SameLine();
    if (ImGui::Button("Backward", button_control_size))
    {
        move_active_cells(direction::backward);
    }
    ImGui::SameLine();
    if (ImGui::Button("Right", button_control_size))
    {
        move_active_cells(direction::right);
    }

    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(window_rotate_width, window_rotate_height));
    ImGui::SetNextWindowPos(ImVec2(window_rotate_x, window_rotate_y));

    ImGui::Begin("Rotate",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowFontScale(4);
    if (ImGui::Button("Rotate X \naxis", button_rotate_size))
    {
        rotate_around(axis::x);
    }
    if (ImGui::Button("Rotate Y \naxis", button_rotate_size))
    {
        rotate_around(axis::y);
    }
    if (ImGui::Button("Rotate Z \naxis", button_rotate_size))
    {
        rotate_around(axis::z);
    }
    ImGui::End();
#endif
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

    // render

    for (cell* c : cells)
    {
        figure_cube->set_scale(8. / cells_max, 8. / cells_max, 8. / cells_max);
        figure_cube->set_translate(
            vector3d(-1. / 2. + (c->get_position().x + 0.5) / cells_max,
                     (c->get_position().z + 0.5) / cells_max,
                     -1. / 2. + (c->get_position().y + 0.5) / cells_max));
        figure_cube->set_texture(textures_block[c->get_texture_index()]);
        figure_cube->uniform_link(uniforms);

        my_engine->reload_uniform();
        my_engine->render_triangles(vertex_buff,
                                    index_buff,
                                    figure_cube->get_texture(),
                                    0,
                                    index_buff->size());
    }
    delete vertex_buff;
    delete index_buff;
}
void game_tetris::start_game()
{
    score            = 0;
    state.is_started = true;
    state.is_restart = false;

    add_primitive();
}

void game_tetris::lose_game()
{
    state.is_started = false;
    state.is_restart = true;
    for (cell* c : cells)
    {
        delete c;
    }
    cells.resize(0);
}

void game_tetris::add_primitive()
{
    static const int   x = cells_max / 2;
    static const int   y = cells_max / 2;
    std::vector<cell*> cells_to_add;

    switch (rand() % 3)
    {
        case 0:
            cells_to_add = gen_primitive_1(x, y);
            break;
        case 1:
            cells_to_add = gen_primitive_2(x, y);
            break;
        case 2:
            cells_to_add = gen_primitive_3(x, y);
            break;
        case 3:
            cells_to_add = gen_primitive_4(x, y);
            break;
    }

    for (cell* c : cells_to_add)
    {
        cells.push_back(c);
        find_near(c);
    }
    active_primitive = new primitive(cells_to_add[0]);
}

std::vector<cell*> game_tetris::gen_primitive_1(int x, int y)
{
    uint8_t index     = rand() % textures_block.size();
    cell*   root_cell = new cell(
        cell::position{ static_cast<int>(x), static_cast<int>(y), cells_max_z },
        index);
    cell* cell_1 = new cell(cell::position{ static_cast<int>(x),
                                            static_cast<int>(y),
                                            cells_max_z - 1 },
                            index);
    cell* cell_2 = new cell(cell::position{ static_cast<int>(x + 1),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    cell* cell_3 = new cell(cell::position{ static_cast<int>(x - 1),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    return std::vector<cell*>{ root_cell, cell_1, cell_2, cell_3 };
}

std::vector<cell*> game_tetris::gen_primitive_2(int x, int y)
{
    uint8_t index     = rand() % textures_block.size();
    cell*   root_cell = new cell(
        cell::position{ static_cast<int>(x), static_cast<int>(y), cells_max_z },
        index);
    cell* cell_1 = new cell(cell::position{ static_cast<int>(x - 1),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    cell* cell_2 = new cell(cell::position{ static_cast<int>(x + 1),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    cell* cell_3 = new cell(cell::position{ static_cast<int>(x + 2),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    return std::vector<cell*>{ root_cell, cell_1, cell_2, cell_3 };
}

std::vector<cell*> game_tetris::gen_primitive_3(int x, int y)
{
    uint8_t index     = rand() % textures_block.size();
    cell*   root_cell = new cell(
        cell::position{ static_cast<int>(x), static_cast<int>(y), cells_max_z },
        index);
    cell* cell_1 = new cell(cell::position{ static_cast<int>(x),
                                            static_cast<int>(y),
                                            cells_max_z - 1 },
                            index);
    cell* cell_2 = new cell(cell::position{ static_cast<int>(x + 1),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    cell* cell_3 = new cell(cell::position{ static_cast<int>(x + 2),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    return std::vector<cell*>{ root_cell, cell_1, cell_2, cell_3 };
}

std::vector<cell*> game_tetris::gen_primitive_4(int x, int y)
{
    uint8_t index     = rand() % textures_block.size();
    cell*   root_cell = new cell(
        cell::position{ static_cast<int>(x), static_cast<int>(y), cells_max_z },
        index);
    cell* cell_1 = new cell(cell::position{ static_cast<int>(x),
                                            static_cast<int>(y),
                                            cells_max_z - 1 },
                            index);
    cell* cell_2 = new cell(cell::position{ static_cast<int>(x + 1),
                                            static_cast<int>(y),
                                            cells_max_z },
                            index);
    cell* cell_3 = new cell(cell::position{ static_cast<int>(x + 1),
                                            static_cast<int>(y),
                                            cells_max_z - 1 },
                            index);
    return std::vector<cell*>{ root_cell, cell_1, cell_2, cell_3 };
}

bool game_tetris::move_active_cells(direction dir)
{
    dir = static_cast<direction>(
        (static_cast<int>(dir) +
         static_cast<int>(M_PI / 2 + (2 * M_PI + camera_angle) / (M_PI / 2))) %
        4);

    std::vector<cell*> visited;
    if (check_moving(active_primitive->get_root(), dir, visited))
    {
        for (cell* c : cells)
        {
            if (!c->get_moving())
                continue;
            cell::position cur_pos = c->get_position();
            switch (dir)
            {
                case direction::left:
                    cur_pos.x--;
                    break;
                case direction::forward:
                    cur_pos.y++;
                    break;
                case direction::right:
                    cur_pos.x++;
                    break;
                case direction::backward:
                    cur_pos.y--;
                    break;
            }
            c->set_position(cur_pos);
            find_near(c);
        }
        return true;
    }
    return false;
}

bool game_tetris::rotate_around(axis ax)
{
    auto get_rotate_pos = [&](cell::position cur_pos)
    {
        cell::position new_pos;
        switch (ax)
        {
            case axis::x:
                new_pos.x = cur_pos.x;
                new_pos.y =
                    cos(M_PI / 2) * cur_pos.y - sin(M_PI / 2) * cur_pos.z;
                new_pos.z =
                    sin(M_PI / 2) * cur_pos.y + cos(M_PI / 2) * cur_pos.z;
                break;
            case axis::y:
                new_pos.x =
                    cos(M_PI / 2) * cur_pos.x + sin(M_PI / 2) * cur_pos.z;
                new_pos.y = cur_pos.y;
                new_pos.z =
                    -sin(M_PI / 2) * cur_pos.x + cos(M_PI / 2) * cur_pos.z;
                break;
            case axis::z:
                new_pos.x =
                    cos(M_PI / 2) * cur_pos.x - sin(M_PI / 2) * cur_pos.y;
                new_pos.y =
                    sin(M_PI / 2) * cur_pos.x + cos(M_PI / 2) * cur_pos.y;
                new_pos.z = cur_pos.z;
                break;
        }
        return new_pos;
    };

    cell::position center_position =
        active_primitive->get_root()->get_position();
    std::vector<cell*> rotate_cells;
    std::vector<cell*> static_cells;
    for (cell* c : cells)
    {
        if (c == active_primitive->get_root())
            continue;
        if (c->get_moving())
        {
            rotate_cells.push_back(c);
        }
        else
        {
            static_cells.push_back(c);
        }
    }
    for (cell* c : rotate_cells)
    {
        cell::position new_pos;
        cell::position cur_pos = c->get_position() - center_position;

        new_pos = get_rotate_pos(cur_pos) + center_position;

        if (new_pos.x < 0 || new_pos.y < 0 || new_pos.z < 0 ||
            (new_pos.x > cells_max - 1) || (new_pos.y > cells_max - 1))
        {
            return false;
        }

        for (cell* c : static_cells)
        {
            if (c->get_position() == new_pos)
            {
                return false;
            }
        }
    }

    for (cell* c : rotate_cells)
    {
        cell::position new_pos;
        cell::position cur_pos = c->get_position() - center_position;

        new_pos = get_rotate_pos(cur_pos) + center_position;

        c->set_position(new_pos);
    }

    return true;
}

bool game_tetris::check_moving(cell*               c,
                               direction           dir,
                               std::vector<cell*>& visited)
{
    for (cell* c : cells)
    {
        if (c->get_moving() && c->get_cell_near(dir) &&
            !c->get_cell_near(dir)->get_moving())
            return false;
        if (c->get_moving())
        {
            if (dir == direction::left && c->get_position().x == 0)
                return false;
            else if (dir == direction::right &&
                     c->get_position().x == cells_max - 1)
                return false;
            else if (dir == direction::forward &&
                     c->get_position().y == cells_max - 1)
                return false;
            else if (dir == direction::backward && c->get_position().y == 0)
                return false;
        }
    }
    return true;
}

void game_tetris::find_near(cell* c)
{
    for (int dir = 0; dir != static_cast<int>(direction::last); dir++)
        c->set_cell_near(static_cast<direction>(dir), nullptr);
    cell::position cur_pos = c->get_position();
    for (cell* near : cells)
    {
        if (near == c)
            continue;
        cell::position near_pos = near->get_position();

        if (cur_pos - cell::position(1, 0, 0) == near_pos)
            c->set_cell_near(direction::left, near);
        if (cur_pos + cell::position(1, 0, 0) == near_pos)
            c->set_cell_near(direction::right, near);
        if (cur_pos - cell::position(0, 1, 0) == near_pos)
            c->set_cell_near(direction::backward, near);
        if (cur_pos + cell::position(0, 1, 0) == near_pos)
            c->set_cell_near(direction::forward, near);
        if (cur_pos - cell::position(0, 0, 1) == near_pos)
            c->set_cell_near(direction::down, near);
        if (cur_pos + cell::position(0, 0, 1) == near_pos)
            c->set_cell_near(direction::up, near);
    }
}

void game_tetris::collision()
{

    for (cell* c : cells)
    {
        c->set_moving(false);
    }
    check_layer();
    add_primitive();
}

void game_tetris::check_layer()
{
    std::array<uint8_t, cells_max_z> count{ 0 };
    std::vector<uint8_t>             z_to_delete;
    for (cell* c : cells)
    {
        count[c->get_position().z]++;
    }
    for (uint8_t i = 0; i < cells_max_z; i++)
    {
        if (i > cells_z_lose && count[i] != 0)
            lose_game();
        if (count[i] == cells_max * cells_max)
            z_to_delete.push_back(i);
    }
    for (uint8_t z : z_to_delete)
    {
        score++;
        for (int j = cells.size() - 1; j >= 0; j--)
        {
            if (cells[j]->get_position().z == z)
            {
                delete cells[j];
                cells.erase(cells.begin() + j, cells.begin() + j + 1);
            }
            else if (cells[j]->get_position().z > z)
            {
                cells[j]->set_position(cells[j]->get_position() -
                                       cell::position(0, 0, 1));
            }
        }
    }
}

bool game_tetris::get_quit_state() const
{
    return state.is_quit;
}
