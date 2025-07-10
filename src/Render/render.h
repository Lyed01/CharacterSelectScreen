#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "../Character/character.h"

struct GridConfig {
    int columnas;
    int startX, startY;
    int cellWidth, cellHeight;
};

void renderText(SDL_Renderer* ren, TTF_Font* font, int x, int y, const std::string& texto);
void renderCharactersGrid(SDL_Renderer* ren, TTF_Font* font, TTF_Font* font2,
    const std::vector<Character>& personajes, int seleccionado,
    const GridConfig& grid, int offsetFila, int offsetCol,
    int ventanaAlto, int ventanaAncho,
    float highlightX, float highlightY);
void renderCharacterDetails(SDL_Renderer* ren, TTF_Font* font, TTF_Font* font2, const Character& personaje);
