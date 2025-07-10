// === INCLUDES DE COMPONENTES PROPIOS ===
#include "../Render/render.h"
#include "../Input/input.h"
#include "../Character/character.h"

// === LIBRERÍAS DE SDL Y C++ ===
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

// === VARIABLES GLOBALES ===
Uint32 lastSelectTime = 0; // Para evitar múltiples selecciones por rebote
SDL_Window* win = nullptr;
SDL_Renderer* ren = nullptr;
TTF_Font* font = nullptr;
TTF_Font* font2 = nullptr;
Mix_Music* musicaFondo = nullptr;
Mix_Chunk* sonidoMover = nullptr;
Mix_Chunk* sonidoSeleccionar = nullptr;
bool musicaYaIniciada = false;

// === ACTUALIZA LA SELECCIÓN EN UN GRID DE PERSONAJES CON TECLAS ===
int actualizarSeleccion(const InputState& input, int idx, int total, int cols) {
    int rows = (total + cols - 1) / cols; // Redondeo hacia arriba para filas
    int row = idx / cols;
    int col = idx % cols;

    // Moverse con flechas ↑ ↓ ← →
    if (input.up) {
        row = (row - 1 + rows) % rows; // wrap-around hacia arriba
    }
    else if (input.down) {
        row = (row + 1) % rows; // wrap-around hacia abajo
    }
    else if (input.left) {
        if (col == 0) {
            // Si estamos en el primer ítem de la fila, vamos al último de la anterior
            idx = (row == 0) ? total - 1 : std::min((row - 1) * cols + (cols - 1), total - 1);
            return idx;
        }
        idx--;
        return idx;
    }
    else if (input.right) {
        if (col == cols - 1 || idx == total - 1) {
            // Si estamos al final de fila o en el último, volvemos al principio o siguiente fila
            idx = (row == rows - 1) ? 0 : std::min((row + 1) * cols, total - 1);
            return idx;
        }
        idx++;
        return idx;
    }

    // Recalculamos el índice por fila y columna
    int newIdx = row * cols + col;
    return (newIdx >= total) ? total - 1 : newIdx;
}

// === INICIALIZA EL AUDIO Y CARGA LOS EFECTOS ===
bool initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Error al iniciar SDL_mixer: %s", Mix_GetError());
        return false;
    }

    // Cargamos música y sonidos
    musicaFondo = Mix_LoadMUS("assets/audio/menu_music.ogg");
    sonidoMover = Mix_LoadWAV("assets/audio/move.wav");
    sonidoSeleccionar = Mix_LoadWAV("assets/audio/select.wav");

    if (!musicaFondo || !sonidoMover || !sonidoSeleccionar) {
        SDL_Log("Error cargando audio: %s", Mix_GetError());
        return false;
    }

    return true;
}

// === INICIALIZA SDL, FUENTES, VENTANA Y RENDERER ===
bool iniciarSDL(SDL_Window** win, SDL_Renderer** ren, TTF_Font** font, TTF_Font** font2) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "Error SDL_Init: " << SDL_GetError() << endl;
        return false;
    }

    if (TTF_Init() != 0) {
        cerr << "Error TTF_Init: " << TTF_GetError() << endl;
        SDL_Quit();
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cerr << "Error IMG_Init: " << IMG_GetError() << endl;
        TTF_Quit(); SDL_Quit();
        return false;
    }

    // Creamos la ventana principal
    *win = SDL_CreateWindow("Selector", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    if (!*win) {
        cerr << "Error SDL_CreateWindow: " << SDL_GetError() << endl;
        TTF_Quit(); IMG_Quit(); SDL_Quit();
        return false;
    }

    // Creamos el renderer para dibujar todo
    *ren = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED);
    if (!*ren) {
        cerr << "Error SDL_CreateRenderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(*win); TTF_Quit(); IMG_Quit(); SDL_Quit();
        return false;
    }

    // Cargamos fuentes
    *font = TTF_OpenFont("assets/fonts/OpenSans-Regular.ttf", 20);
    *font2 = TTF_OpenFont("assets/fonts/team401.ttf", 40);
    if (!*font || !*font2) {
        cerr << "Error TTF_OpenFont: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(*ren); SDL_DestroyWindow(*win); TTF_Quit(); IMG_Quit(); SDL_Quit();
        return false;
    }

    return true;
}

// === LIBERA TODA LA MEMORIA Y RECURSOS USADOS ===
void limpiar(SDL_Window* win, SDL_Renderer* ren, TTF_Font* font, vector<Character>& personajes) {
    for (auto& c : personajes) {
        if (c.texture) SDL_DestroyTexture(c.texture);
    }
    if (font) TTF_CloseFont(font);
    if (ren) SDL_DestroyRenderer(ren);
    if (win) SDL_DestroyWindow(win);
    TTF_Quit(); IMG_Quit(); SDL_Quit();
}

// === CARGA LOS PERSONAJES DESDE UN .TXT EN FORMATO CSV ===
vector<Character> cargarPersonajes(const string& rutaArchivo, SDL_Renderer* ren) {
    vector<Character> personajes;
    ifstream file(rutaArchivo);
    if (!file.is_open()) {
        cerr << "No se pudo abrir: " << rutaArchivo << endl;
        return personajes;
    }

    string linea;
    while (getline(file, linea)) {
        if (linea.empty() || linea[0] == '#') continue; // Ignoramos líneas vacías o comentarios

        stringstream ss(linea);
        string nombre, vidaStr, fuerzaStr, velocidadStr, descripcion, imgPath;

        // Leemos los campos separados por comas
        getline(ss, nombre, ',');
        getline(ss, vidaStr, ',');
        getline(ss, fuerzaStr, ',');
        getline(ss, velocidadStr, ',');

        // Descripción puede venir entre comillas
        if (ss.peek() == '"') {
            ss.get(); // Salteamos la primera comilla
            getline(ss, descripcion, '"'); // Leemos hasta la segunda comilla
            ss.ignore(1); // Salteamos la coma que sigue
        }

        getline(ss, imgPath); // Ruta de la imagen

        Character c;
        c.name = nombre;
        c.vida = stoi(vidaStr);
        c.fuerza = stoi(fuerzaStr);
        c.velocidad = stoi(velocidadStr);
        c.descripcion = descripcion;
        c.imagePath = imgPath;
        c.texture = IMG_LoadTexture(ren, c.imagePath.c_str());
        if (!c.texture) {
            cerr << "Error cargando imagen de " << c.name << ": " << IMG_GetError() << endl;
        }

        personajes.push_back(std::move(c));
    }
    
    return personajes;
}

// === BUCLE PRINCIPAL DEL JUEGO/SELECTOR ===
void runGame() { 

    
    if (!iniciarSDL(&win, &ren, &font, &font2)) return;
    if (!initAudio()) return;

    vector<Character> personajes = cargarPersonajes("assets/characters.txt", ren);
    if (personajes.empty()) {
        limpiar(win, ren, font, personajes);
        return;
    }

    
    // prueba control
    cout << "Cantidad de personajes cargados: " << personajes.size() << endl;
    if (!personajes.empty()) {
        cout << "Primer personaje: " << personajes[0].name << " con " << personajes[0].vida << " HP." << endl;
    }



    // Configuración del grid de selección
    GridConfig grid{ 4, 150, 150, 175, 175 }; // columnas, tamaño celdas, márgenes
    InputState input{};
    int seleccionado = 0, offsetFila = 0, offsetCol = 0, alto = 720, ancho = 1240;
    float animX = grid.startX, animY = grid.startY, speed = 0.15f;
    Uint32 cooldownMs = 150;
    Uint32 lastMoveTime = 0;
    Uint32 lastSelectTime = 0;

    enum class GameState { SELECCION, DETALLE };
    GameState estado = GameState::SELECCION;
    GameState estadoAnterior = GameState::DETALLE;

    // Volumen general
    Mix_VolumeMusic(50);
    Mix_VolumeChunk(sonidoMover, 60);
    Mix_VolumeChunk(sonidoSeleccionar, 80);

    while (true) {
        processInput(input);     // Detecta input del teclado
        if (input.quit) break;

        Uint32 currentTime = SDL_GetTicks();

        // Control de música según estado
        if (estado != estadoAnterior) {
            if (estado == GameState::SELECCION && !Mix_PlayingMusic()) {
                Mix_PlayMusic(musicaFondo, -1);
            }
            else if (estado == GameState::DETALLE) {
                Mix_HaltMusic();
            }
            estadoAnterior = estado;
        }

        // === INPUT GENERAL SEGÚN ESTADO ===
        if (estado == GameState::SELECCION) {
            if ((input.up || input.down || input.left || input.right) && currentTime - lastMoveTime > cooldownMs) {
                int anterior = seleccionado;
                seleccionado = actualizarSeleccion(input, seleccionado, personajes.size(), grid.columnas);
                lastMoveTime = currentTime;
                if (seleccionado != anterior) Mix_PlayChannel(-1, sonidoMover, 0);
            }

            if (input.select && currentTime - lastSelectTime > cooldownMs) {
                estado = GameState::DETALLE;
                lastSelectTime = currentTime;
                Mix_PlayChannel(-1, sonidoSeleccionar, 0);
            }

        }
        else if (estado == GameState::DETALLE) {
            if (input.select && currentTime - lastSelectTime > cooldownMs) {
                estado = GameState::SELECCION;
                lastSelectTime = currentTime;
                Mix_PlayChannel(-1, sonidoSeleccionar, 0);
            }
        }

        // === SCROLL AUTOMÁTICO ===
        int fila = seleccionado / grid.columnas;
        int col = seleccionado % grid.columnas;
        int filasVisibles = (alto - grid.startY) / grid.cellHeight;
        int columnasVisibles = (ancho - grid.startX) / grid.cellWidth;

        if (fila < offsetFila) offsetFila = fila;
        else if (fila >= offsetFila + filasVisibles) offsetFila = fila - filasVisibles + 1;

        if (col < offsetCol) offsetCol = col;
        else if (col >= offsetCol + columnasVisibles) offsetCol = col - columnasVisibles + 1;

        // === ANIMACIÓN DEL CURSOR CON INTERPOLACIÓN SUAVE ===
        float tgtX = grid.startX + (col - offsetCol) * grid.cellWidth;
        float tgtY = grid.startY + (fila - offsetFila) * grid.cellHeight;
        animX += (tgtX - animX) * speed;
        animY += (tgtY - animY) * speed;

        // === RENDER DE LA PANTALLA ===
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); // fondo negro
        SDL_RenderClear(ren);

        if (estado == GameState::SELECCION) {
            renderCharactersGrid(ren, font, font2, personajes, seleccionado, grid, offsetFila, offsetCol, alto, ancho, animX, animY);
        }
        else {
            renderCharacterDetails(ren, font, font2, personajes[seleccionado]);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16); // ~60 FPS
    }

    limpiar(win, ren, font, personajes);
}
