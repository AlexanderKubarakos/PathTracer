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

void sphereScene(Scene* scene, int factor)
{
    Material* ground_material = (Material*)createLambertian((Color){0.5, 0.5, 0.5});
    addHittable(scene, createSphere((Vec3){0,-1000,0}, 1000, ground_material));

    for (int a = -factor; a < factor; a++) {
        for (int b = -factor; b < factor; b++) {
            double choose_mat = randomDouble();
            Vec3 center = (Vec3){a + 0.9*randomDouble(), 0.2, b + 0.9*randomDouble()};

            if ((length(subVec3(center, (Vec3){4, 0.2, 0}))) > 0.9) {
                Material* sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    Color albedo = mulVec3Vec3(randomColor(), randomColor());
                    sphere_material = (Material*)createLambertian(albedo);
                    addHittable(scene, createSphere(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    Color albedo = randomColor(0.5, 1);
                    double fuzz = randomDoubleRange(0, 0.5);
                    sphere_material = (Material*)createMetal(albedo, fuzz);
                    addHittable(scene, createSphere(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = (Material*)createDielectric(1.5);
                    addHittable(scene, createSphere(center, 0.2, sphere_material));
                }
            }
        }
    }

    Material* material1 = (Material*)createDielectric(1.5);
    addHittable(scene,createSphere((Vec3){0,1,0}, 1.0, material1));

    Material* material2 = (Material*)createLambertian((Color){0.4, 0.2, 0.1});
    addHittable(scene,createSphere((Vec3){-4, 1, 0}, 1.0, material2));

    Material* material3 = (Material*)createMetal((Color){0.7, 0.6, 0.5}, 0.0);
    addHittable(scene,createSphere((Vec3){4, 1, 0}, 1.0, material3));
}

int main()
{
    const int width = 1080;
    const double aspectRatio = 16.0/9.0;
    // Create scene of hittable objects
    Scene* scene = createScene(512);
    sphereScene(scene, 1);

    SDLInit(SDL_INIT_EVERYTHING);
    SDLWindow window;
    if (!createWindow(&window, width, width / aspectRatio, "Raytracer"))
    {
        return -1;
    }

    // Create camera to render scenes
    Camera* camera = createCamera(width, aspectRatio, 100, 50, 20, 0.6, 10.0);
    lookAt(camera, (Vec3){13,2,3}, (Vec3){0,0,0}, (Vec3){0,1,0});

    RenderResult* result = renderScene(camera, scene, 6); // Start render

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
    while(!SDLShouldWindowClose(&window))
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
    result->stop = true;
    while (!result->done);

    printf("Cleaning up\n");
    destroyCamera(camera);
    destroyScene(scene);

    printf("Finished\n");
    return 0;
}
