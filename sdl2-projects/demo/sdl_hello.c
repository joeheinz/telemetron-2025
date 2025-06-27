#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to init SDL: %s", SDL_GetError());
        return 1;
    }

    atexit(SDL_Quit);

    SDL_Window *win = SDL_CreateWindow("SDL2 Hello",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        480, 320, SDL_WINDOW_SHOWN);

    if (!win) {
        SDL_Log("Could not create window: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        SDL_Log("Could not create renderer: %s", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Log("Ventana mostrada. Esperando 5 segundos...");
    SDL_Delay(5000);  // mostrar ventana durante 5 segundos

    // NO destruyas nada, deja que el sistema lo libere
    return 0;
}

