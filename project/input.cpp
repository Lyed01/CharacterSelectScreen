#include "input.h"

void processInput(InputState& input) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            input.quit = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_UP: input.up = true; break;
            case SDLK_w: input.up = true; break;
            case SDLK_DOWN: input.down = true; break;
            case SDLK_s: input.down = true; break;
            case SDLK_LEFT: input.left = true; break;
            case SDLK_a: input.left = true; break;
            case SDLK_RIGHT: input.right = true; break;
            case SDLK_d: input.right = true; break;
            case SDLK_RETURN: input.select = true; break;
            case SDLK_ESCAPE: input.quit = true; break;
            
            }
        }
        else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            case SDLK_UP: input.up = false; break;
            case SDLK_w: input.up = false; break;
            case SDLK_DOWN: input.down = false; break;
            case SDLK_s: input.down = false; break;
            case SDLK_LEFT: input.left = false; break;
            case SDLK_a: input.left = false; break;
            case SDLK_RIGHT: input.right = false; break;
            case SDLK_d: input.right = false; break;
            case SDLK_RETURN: input.select = false; break;
            case SDLK_ESCAPE: input.quit = false; break;
            }
        }
    }
}
