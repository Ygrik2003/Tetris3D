#include "game.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>

class android_redirected_buf : public std::streambuf
{
public:
    android_redirected_buf() = default;

private:
    // This android_redirected_buf buffer has no buffer. So every character
    // "overflows" and can be put directly into the teed buffers.
    int overflow(int c) override
    {
        if (c == EOF)
        {
            return !EOF;
        }
        else
        {
            if (c == '\n')
            {
#ifdef __ANDROID__
                // android log function add '\n' on every print itself
                __android_log_print(
                    ANDROID_LOG_ERROR, "GameActivity", "%s", message.c_str());
#else
                std::printf("%s\n", message.c_str()); // TODO test only
#endif
                message.clear();
            }
            else
            {
                message.push_back(static_cast<char>(c));
            }
            return c;
        }
    }

    int sync() override { return 0; }

    std::string message;
};

int main(int argc, char* argv[])
{
    android_redirected_buf logcat;

    std::cout.rdbuf(&logcat);
    std::cerr.rdbuf(&logcat);
    std::clog.rdbuf(&logcat);

    config      cfg;
    game_tetris my_game;

    if (!my_game.initialize(cfg))
        return -1;

    event e{};

    auto time_last = std::chrono::steady_clock::now() - std::chrono::seconds(1);
    while (my_game.event_listener(e) && !my_game.get_quit_state())
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