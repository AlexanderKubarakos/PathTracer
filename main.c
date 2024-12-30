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
int main()
{
    // Create scene of hittable objects
    Scene* scene = createScene(512);

    Material* ground_material = (Material*)createLambertian((Color){0.5, 0.5, 0.5});
    addHittable(scene, createSphere((Vec3){0,-1000,0}, 1000, ground_material));

    for (int a = -1; a < 1; a++) {
        for (int b = -1; b < 1; b++) {
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

    // Create camera to render scenes
    Camera* camera = createCamera(1080, 16.0/9.0, 500, 50, 20, 0.6, 10.0);
    lookAt(camera, (Vec3){13,2,3}, (Vec3){0,0,0}, (Vec3){0,1,0});
    renderScene(camera, scene);

    destroyCamera(camera);
    destroyScene(scene);

    /*
    destroyMaterial((Material*)lambertian1);
    destroyMaterial((Material*)lambertian2);
    destroyMaterial((Material*)dielectric);
    destroyMaterial((Material*)metal2);
    */
    printf("Finished\n");
    return 0;
}