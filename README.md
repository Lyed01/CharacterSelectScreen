# Selector de Personajes SDL2

## Descripción

Este programa es un selector de personajes desarrollado en C++ utilizando SDL2 y sus extensiones (SDL_ttf, SDL_image, SDL_mixer). Permite navegar en un grid de personajes cargados desde un archivo CSV, escuchar música de fondo y sonidos de selección, y ver detalles de cada personaje.

Funciona con teclado y muestra animaciones suaves para el cursor de selección.

---

## Características principales

- Carga personajes desde un archivo CSV con atributos y ruta de imagen.  
- Navegación con las flechas del teclado en un grid dinámico.  
- Animación suave del cursor de selección.  
- Cambio de estado entre selección y detalle del personaje.  
- Música de fondo y efectos de sonido con SDL_mixer.  
- Limpieza adecuada de recursos SDL.  

---

## Requisitos

- SDL2  
- SDL2_ttf  
- SDL2_image  
- SDL2_mixer  
- Compilador C++ con soporte C++11 o superior  
- Archivo `assets/characters.txt` con formato CSV válido  
- Archivos de audio en `assets/audio/` y fuentes en `assets/fonts/`  

---

## Formato del archivo `characters.txt`

Cada línea define un personaje con campos separados por comas:

    nombre,vida,fuerza,velocidad,"descripción opcional entre comillas",ruta_imagen
    
Ejemplo:

    Mario,100,50,30,"El fontanero valiente",assets/img/mario.png

## Estructura del código

runGame(): Función principal que inicia SDL, carga recursos, maneja el loop principal y la lógica de selección.

cargarPersonajes(): Lee el archivo CSV y genera un vector de personajes con sus atributos y texturas.

actualizarSeleccion(): Controla el movimiento del cursor en el grid.

iniciarSDL(), initAudio(): Inicializan SDL y el audio.

limpiar(): Libera recursos usados.

renderCharactersGrid(), renderCharacterDetails(): Funciones de renderizado (implementación separada).

## Notas

Asegúrate de tener los assets (imágenes, audio, fuentes) en las rutas correctas.

El programa usa interpolación para animar el cursor, para mejor experiencia visual.

Actualmente soporta teclado para navegar y seleccionar personajes.



