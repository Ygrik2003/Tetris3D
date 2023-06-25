#include "game.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    config cfg;
    cfg.shader_fragment = "./99-game/shaders/shader.frag";
    cfg.shader_vertex   = "./99-game/shaders/shader.vert";
    cfg.texture_block   = "./99-game/textures/texture_block.png";
    cfg.texture_board   = "./99-game/textures/texture_board.png";

    game_tetris my_game;
    if (!my_game.initialize(cfg))
    {
        return -1;
    }

    event e{};
    float phi      = 0;
    auto time_last = std::chrono::steady_clock::now() - std::chrono::seconds(1);
    while (my_game.event_listener(e))
    {

        auto time_now = std::chrono::steady_clock::now();
        if ((time_now - time_last).count() > 1.e9 / fps)
        {
            my_game.update();
            my_game.render();
        }
    }

    return EXIT_SUCCESS;
}