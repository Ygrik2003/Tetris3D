#include "game.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    config      cfg;
    game_tetris my_game;

    if (!my_game.initialize(cfg))
        return -1;

    event e{};

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