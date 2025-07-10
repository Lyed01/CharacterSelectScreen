#pragma once
#include <SDL.h>

struct InputState {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool select = false;
    bool quit = false;
};

void processInput(InputState& input);
