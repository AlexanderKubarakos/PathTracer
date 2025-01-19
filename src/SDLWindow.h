#pragma once

#include "SDL.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    SDL_Surface* surface;
    SDL_Window* window;
    bool running;
} SDLWindow;

bool SDLInit(uint flags);
void SDLQuit();
bool createWindow(SDLWindow* window, int width, int height, const char* name);
void deleteWindow(SDLWindow* window);
SDL_Surface* getSDLSurface(SDLWindow* window);
SDL_Window* getSDLWindow(SDLWindow* window);
void SDLPollEvents(SDLWindow* window);
bool SDLShouldWindowClose(SDLWindow* window);
void SDLUpdate(SDLWindow* window);
