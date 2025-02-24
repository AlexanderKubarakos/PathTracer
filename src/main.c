#include "Camera.h"
#include "Dielectric.h"
#include "Hittable.h"
#include "KubsMath.h"
#include "Material.h"
#include "PPM.h"
#include "Vec3.h"
#include <stdio.h>
#include <stdbool.h>
#include "Sphere.h"
#include "Scene.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"
#include "SDLWindow.h"
#include "SDL_surface.h"
#include <pthread.h>
#include "DiffuseLight.h"
#define _GNU_SOURCE  
#include <sched.h>
#include "Model.h"
#include "SceneSelector.h"

#define SAMPLE 500
#define BONCE_DEPTH 50

int main(int argc, char** argv)
{
    bool openWindow = true;
    if(argc == 2 && strcmp(argv[1], "-noWindow") == 0)
        openWindow = false; 

    // Image generation

    const int width = 800;
    const double aspectRatio = 16.0/9.0;
    // Create camera to render scenes
    Camera* camera = createCamera(width, aspectRatio, SAMPLE, BONCE_DEPTH, 20, 0, 10.0);
    // Create scene of hittable objects
    Scene* scene = createScene(128, (Color){1,1,1});
    selectScene(scene, camera, 2);

    // Windowing

    SDLInit(SDL_INIT_EVERYTHING);
    SDLWindow window;
    if (!createWindow(&window, width, width / aspectRatio, "Raytracer"))
    {
        return -1;
    }

    RenderResult* result = renderScene(camera, scene, 16); // Start render
    SDL_Surface* fromImage;
    fromImage = SDL_CreateRGBSurfaceFrom(result->image->data, camera->width,
                        camera->height, 8 * 3, 3 * camera->width, 0x0000FF,0x00FF00,0xFF0000,0);
    if (!fromImage)
        printf("Failed to create image: %s\n", SDL_GetError());
    SDL_Rect dest;
    dest.x = 0;
    dest.y = 0;
    dest.w = camera->width;
    dest.h = camera->height;
    while(!SDLShouldWindowClose(&window) && openWindow)
    {
        pthread_mutex_lock(&result->image->mutex);
        if(SDL_BlitScaled(fromImage, NULL, getSDLSurface(&window), &dest) < 0)
            printf("Failed to blit!\n");
        pthread_mutex_unlock(&result->image->mutex);

        // End Frame
        SDLPollEvents(&window);
        SDLUpdate(&window);
    }

    SDLQuit();
    if (openWindow)
        result->stop = true;
    while (!result->done);

    printf("Cleaning up\n");
    //destroyCamera(camera);
    //destroyScene(scene);

    printf("Finished\n");
    return 0;
}
