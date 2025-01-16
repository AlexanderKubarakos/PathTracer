#include "SDLWindow.h"

bool SDLInit(uint flags)
{
    if (SDL_Init(flags) < 0)
    {
        printf("ERROR: SDL Init ");
        printf("%s\n", SDL_GetError());
        return false;
    }
    return true;
}

void SDLQuit()
{
    SDL_Quit();
}

bool createWindow(SDLWindow* window)
{
    if (!window)
    {
        printf("ERROR: Trying to create window at null pointer\n");
        return false;
    }

    window->surface = NULL;
    window->window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                            SDL_WINDOW_SHOWN);
    window->running = true;
    if (!window->window)
    {
        printf("ERROR: Window Creation\n");
        return false;
    }

    window->surface = SDL_GetWindowSurface(window->window);

    if (!window->surface)
    {
        printf("ERROR: Get Surface\n");
        return false;
    }

    return true;
}

void deleteWindow(SDLWindow* window)
{
    SDL_DestroyWindow(window->window);
    free(window);
}

SDL_Surface* getSDLSurface(SDLWindow* window)
{
    return window->surface;
}

SDL_Window* getSDLWindow(SDLWindow* window)
{
    return window->window;
}

void SDLPollEvents(SDLWindow* window)
{
   SDL_Event ev;
   while(SDL_PollEvent(&ev) != 0)
   {
      switch(ev.type)
      {
          case SDL_QUIT:
              window->running = false;
              break;
            default:
                break;
      }
   }
}

bool SDLShouldWindowClose(SDLWindow* window)
{
    return !window->running;
}

void SDLUpdate(SDLWindow* window)
{
    SDL_UpdateWindowSurface(window->window);
}
