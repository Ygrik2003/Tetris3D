#include "game.h"
#include "objects/model.h"

game_tetris::game_tetris()
{
    state.is_started = 0;
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

    window_score_width  = 0.1 * cfg.width;
    window_score_height = 0.03 * cfg.height;
    window_score_x      = 10;
    window_score_y      = 10;

    window_control_width  = 0.30 * cfg.width;
    window_control_height = 0.35 * cfg.height;
    window_control_x      = 20;
    window_control_y      = 0.6 * cfg.height;
    window_rotate_width   = 0.2 * cfg.width;
    window_rotate_height  = 0.95 * cfg.height;
    window_rotate_x       = 0.8 * cfg.width - 10;
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
            if (e.motion.x_rel && state.is_rotated)
            {
#ifdef ANDROID
                if (e.motion.x > window_control_width &&
                    e.motion.x < window_rotate_x)
#endif
                    camera_angle += e.motion.x_rel * M_PI / 300;
            }
            if (e.motion.y_rel && state.is_rotated)
            {
#ifdef ANDROID
                if (e.motion.x > window_control_width &&
                    e.motion.x < window_rotate_x)
                {
#endif
                    view_height += e.motion.y_rel / 50;
                    if (view_height < 1.)
                        view_height = 1.;
                    if (view_height > 5)
                        view_height = 5;
#ifdef ANDROID
                }
#endif
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
    ImGui::NewFrame();
    if (!state.is_started)
    {
        draw_menu();
    }
    else
    {
        shader_scene->use();
        render_scene();
#ifdef ANDROID
        draw_ui();
#endif
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

    // if (ImGui::ImageButton())
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

    // render

    for (cell* c : cells)
    {
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
    state.is_started = true;
    add_primitive();
}

void game_tetris::add_primitive()
{
    static float x         = 0;
    static float y         = 0;
    uint8_t      index     = rand() % textures_block.size();
    cell*        root_cell = new cell(
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
    cells.push_back(root_cell);
    cells.push_back(cell_1);
    cells.push_back(cell_2);
    find_near(root_cell);
    find_near(cell_1);
    find_near(cell_2);
    active_primitive = new primitive(root_cell);

    x++;
    if (x == cells_max)
    {
        x = 0;
        y += 0.5;
        if (y == cells_max)
            y = 0;
    }
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

bool game_tetris::check_moving(cell*               c,
                               direction           dir,
                               std::vector<cell*>& visited)
{
    for (auto find_cell : visited)
        if (c == find_cell)
            return true;
    visited.push_back(c);

    bool is_left     = true;
    bool is_right    = true;
    bool is_forward  = true;
    bool is_backward = true;
    if (c->get_cell_near(direction::left))
    {
        is_left = check_moving(c->get_cell_near(direction::left), dir, visited);
    }
    if (c->get_cell_near(direction::right))
    {
        is_right =
            check_moving(c->get_cell_near(direction::right), dir, visited);
    }
    if (c->get_cell_near(direction::forward))
    {
        is_forward =
            check_moving(c->get_cell_near(direction::forward), dir, visited);
    }
    if (c->get_cell_near(direction::backward))
    {
        is_backward =
            check_moving(c->get_cell_near(direction::backward), dir, visited);
    }

    if (c->get_position().x == 0 && dir == direction::left)
    {
        return false;
    }
    else if (c->get_position().x == (cells_max - 1) && dir == direction::right)
    {
        return false;
    }
    else if (c->get_position().y == (cells_max - 1) &&
             dir == direction::forward)
    {
        return false;
    }
    else if (c->get_position().y == 0 && dir == direction::backward)
    {
        return false;
    }

    if (c->get_cell_near(dir) && !c->get_cell_near(dir)->get_moving())
        return false;

    return is_left && is_right && is_forward && is_backward;
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
    for (cell* c : cells)
    {
    }
}
