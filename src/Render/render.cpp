#include "render.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <cmath>
#include <String>

using namespace std;
// Renderiza texto en la posición (x,y) con color opcional (blanco por defecto)
void renderText(SDL_Renderer* ren, TTF_Font* font, int x, int y, const std::string& texto) {
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surf = TTF_RenderText_Solid(font, texto.c_str(), color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_Rect rect = { x, y, surf->w, surf->h };
    SDL_RenderCopy(ren, tex, nullptr, &rect);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}
void renderCharacterDetails(SDL_Renderer* ren, TTF_Font* font, TTF_Font* font2, const Character& personaje) {
    int startX = 20;
    int startY = 50;
    int lineHeight = 35;
    renderText(ren, font, 950, 650, "Press RETURN key to return");

    // Imagen del personaje
    if (personaje.texture) {
        SDL_Rect imgRect = { 500, 50, 400, 500 };
        SDL_RenderCopy(ren, personaje.texture, nullptr, &imgRect);
    }

    renderText(ren, font2, startX, startY + 0 * lineHeight, ">> " + personaje.name);
    renderText(ren, font, startX, startY + 2 * lineHeight, "Vida: " + to_string(personaje.vida));
    renderText(ren, font, startX, startY + 3 * lineHeight, "Fuerza: " + to_string(personaje.fuerza));
    renderText(ren, font, startX, startY + 4 * lineHeight, "Velocidad: " + to_string(personaje.velocidad));
    renderText(ren, font, startX, startY + 5 * lineHeight, "Desc:");

    const int maxLineLength = 40;
    string desc = personaje.descripcion;
    int currentY = startY + 6 * lineHeight;

    while (!desc.empty()) {
        size_t len = desc.length() > maxLineLength ? desc.find_last_of(' ', maxLineLength) : string::npos;
        if (len == string::npos) len = std::min(desc.length(), (size_t)maxLineLength);
        string line = desc.substr(0, len);
        renderText(ren, font, startX + 20, currentY, line);
        desc.erase(0, len);
        if (!desc.empty() && desc.front() == ' ') desc.erase(0, 1);
        currentY += lineHeight;
    }
}


// Renderiza la grilla de personajes con un selector resaltado en (highlightX, highlightY)
void renderCharactersGrid(SDL_Renderer* ren, TTF_Font* font, TTF_Font* font2 , 
    const std::vector<Character>& personajes, int seleccionado,
    const GridConfig& grid, int offsetFila, int offsetCol,
    int ventanaAlto, int ventanaAncho,
    float highlightX, float highlightY)
{
    const int padding = 10;
    int filas = std::ceil(personajes.size() / (float)grid.columnas);

    for (int i = 0; i < (int)personajes.size(); ++i) {
        int fila = i / grid.columnas;
        int col = i % grid.columnas;
        int drawFila = fila - offsetFila;
        int drawCol = col - offsetCol;

        if (drawFila < 0 || drawCol < 0) continue;

        int x = grid.startX + drawCol * grid.cellWidth;
        int y = grid.startY + drawFila * grid.cellHeight;

        if (y >= ventanaAlto || x >= ventanaAncho) continue;

        SDL_Rect box = { x, y, grid.cellWidth - padding, grid.cellHeight - padding };

        // Fondo del cuadro
        SDL_SetRenderDrawColor(ren, 100, 100, 255, 255);
        SDL_RenderFillRect(ren, &box);

        // Imagen del personaje
        if (personajes[i].texture) {
            SDL_Rect imgRect = {
                box.x + 10,
                box.y + 10,
                box.w - 20,
                box.h - 40
            };
            SDL_RenderCopy(ren, personajes[i].texture, nullptr, &imgRect);
        }
        renderText(ren, font2, 150, 20 , "CHOOSE YOUR CHARACTER");
        renderText(ren, font, 930, 680, "Press ESC to quit");
        renderText(ren, font, 930, 650, "Press RETURN key to select a player");
        renderText(ren, font, 930, 620, "Press WASD or ArrowKeys to move");
        
        // Nombre del personaje
        renderText(ren, font, box.x + 15, box.y + box.h - 25, personajes[i].name);

        // Selector resaltado
        if (i == seleccionado) {
            // Tiempo actual en segundos
            Uint32 ticks = SDL_GetTicks();
            float time = ticks / 1000.0f;

            // Colores entre los que va a parpadear
            SDL_Color colorA = { 255, 0, 0, 255 };      
            SDL_Color colorB = { 255, 255, 0, 255 };    

            // Interpolación oscilante con sin() para un efecto suave
            float t = (std::sin(time * 3.0f) + 1.0f) / 2.0f; // Oscila entre 0 y 1

            // Mezcla de los dos colores
            SDL_Color interpolatedColor = {
                (Uint8)(colorA.r + (colorB.r - colorA.r) * t),
                (Uint8)(colorA.g + (colorB.g - colorA.g) * t),
                (Uint8)(colorA.b + (colorB.b - colorA.b) * t),
                255
            };

            // Aplicar el color interpolado
            SDL_SetRenderDrawColor(ren, interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, interpolatedColor.a);

            // Definir el rectángulo del borde
            SDL_Rect border = {
                (int)highlightX,
                (int)highlightY,
                grid.cellWidth - padding,
                grid.cellHeight - padding
            };

            // Dibujar borde con grosor (3 píxeles)
            for (int thickness = 0; thickness < 3; ++thickness) {
                SDL_Rect thickBorder = {
                    border.x - thickness,
                    border.y - thickness,
                    border.w + thickness * 2,
                    border.h + thickness * 2
                };
                SDL_RenderDrawRect(ren, &thickBorder);
            }
        }
    }
}

