#include "SceneSelector.h"
#include "Sphere.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"
#include "DiffuseLight.h"
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

            if ((length(vec3Sub(center, (Vec3){4, 0.2, 0}))) > 0.9) {
                Material* sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    Color albedo = vec3MulV(randomColor(), randomColor());
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
                    sphere_material = (Material*)createDiffuseLight((Color){1,1,1});
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

void smallDragonScene(Scene* scene, Camera* camera)
{
    scene->background = (Color){1,1,1};
    lookAt(camera, (Vec3){-3,0.2,-1}, (Vec3){0,0,0}, (Vec3){0,1,0});
    
    Model* model = loadOBJModel("models/SmallDragon.obj");
    sceneAddModel(scene, model);
}


void largeDragonScene(Scene* scene, Camera* camera)
{
    scene->background = (Color){1,1,1};
    lookAt(camera, (Vec3){-3,0.2,-1}, (Vec3){0,0,0}, (Vec3){0,1,0});
    
    Model* model = loadOBJModel("models/LargeDragon.obj");
    sceneAddModel(scene, model);
}

void selectScene(Scene* scene, Camera* camera, int id)
{
    switch (id)
    {
        case 0: sphereScene(scene, camera, 7); break;
        case 1: sphereScene2(scene, camera); break;
        case 2: smallDragonScene(scene, camera); break;
        case 3: largeDragonScene(scene, camera); break;
    }
}
