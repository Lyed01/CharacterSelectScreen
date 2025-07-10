// character.h
#pragma once
#include <string>
#include <vector>
#include <SDL.h>

struct Character {
    std::string name;
    int vida;
    int fuerza;
    int velocidad;
    std::string descripcion;
    std::string imagePath;
    SDL_Texture* texture = nullptr;
};

