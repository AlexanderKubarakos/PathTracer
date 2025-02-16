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
void sphereScene(Scene* scene, Camera* camera, int factor)
{
    scene->background = (Color){0,0,0};
    lookAt(camera, (Vec3){13,2,3}, (Vec3){0,0,0}, (Vec3){0,1,0});
    Material* ground_material = (Material*)createLambertian((Color){0.5, 0.5, 0.5});
    addHittable(scene, createSphere((Vec3){0,-100,0}, 100, ground_material));

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

    Material* material1 = (Material*)createDiffuseLight((Color){1,1,1});
    addHittable(scene,createSphere((Vec3){0,1,0}, 1.0, material1));

    Material* material2 = (Material*)createLambertian((Color){0.4, 0.2, 0.1});
    addHittable(scene,createSphere((Vec3){-4, 1, 0}, 1.0, material2));

    Material* material3 = (Material*)createMetal((Color){0.7, 0.6, 0.5}, 0.0);
    addHittable(scene,createSphere((Vec3){4, 1, 0}, 1.0, material3));
}

void sphereScene2(Scene* scene, Camera* camera)
{
    scene->background = (Color){0,0,0};
    lookAt(camera, (Vec3){-8,10,5}, (Vec3){-3.2,4,0}, (Vec3){0,1,0});
    Material* ground_material = (Material*)createLambertian((Color){102/255.0, 51/255.0, 153/255.0});
    addHittable(scene, createSphere((Vec3){1,-14,3}, 15, ground_material));

    Material* light = (Material*)createDiffuseLight((Color){1,1,1});
    addHittable(scene,createSphere((Vec3){2,0,-15}, 7.5, light));

    //Material* black = (Material*)createLambertian((Color){0/255.0, 0/255.0, 0/255.0});
    Material* blue = (Material*)createLambertian((Color){0/255.0, 0/255.0, 255/255.0});
    Material* green = (Material*)createLambertian((Color){0/255.0, 255/255.0, 0/255.0});
    Material* red = (Material*)createLambertian((Color){255/255.0, 0/255.0, 0/255.0});
    Material* white = (Material*)createLambertian((Color){255/255.0, 255/255.0, 255/255.0});

    addHittable(scene,createSphere((Vec3){-2.5,1,-1.2}, 0.6, blue));
    addHittable(scene,createSphere((Vec3){-1,1,-1.15}, 0.65, green));
    addHittable(scene,createSphere((Vec3){0.5,1.2,-1}, 0.7, red));
    addHittable(scene,createSphere((Vec3){2,1.2,-1}, 0.75, white));
}

void dragonScene(Scene* scene, Camera* camera)
{
    Model* model = loadOBJModel("models/SmallDragon.obj");
    addHittable(scene, (Hittable*)model);
    lookAt(camera, (Vec3){-3,0.2,-1}, (Vec3){0,0,0}, (Vec3){0,1,0});
}

int main()
{
    const int width = 800;
    const double aspectRatio = 16.0/9.0;
    // Create camera to render scenes
    Camera* camera = createCamera(width, aspectRatio,10, 50, 20, 0, 10.0);
    // Create scene of hittable objects
    Scene* scene = createScene(128, (Color){1,1,1});
    //sphereScene(scene, camera, 7);
    //sphereScene2(scene, camera);
    dragonScene(scene, camera);

    SDLInit(SDL_INIT_EVERYTHING);
    SDLWindow window;
    if (!createWindow(&window, width, width / aspectRatio, "Raytracer"))
    {
        return -1;
    }

    RenderResult* result = renderScene(camera, scene, 12); // Start render

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
    //destroyCamera(camera);
    //destroyScene(scene);

    printf("Finished\n");
    return 0;
}
