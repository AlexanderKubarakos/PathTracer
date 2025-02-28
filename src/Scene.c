#include "Scene.h"
#include "Hittable.h"
#include "Triangle.h"

#include <stdlib.h>
#include <stdio.h>
Scene* createScene(int length, Color background)
{
    Scene* s = malloc(sizeof(Scene));
    if (!s)
    {
        printf("Failed to allocate memory for scene\n");
    }

    s->list = hittableListCreate();
    s->models = malloc(sizeof(Model) * 16);
    s->background = background;
    return s;
}

void destroyScene(Scene* scene)
{
    for (int i = 0; i < scene->list.size; i++)
    {
        scene->list.list[i]->destroy(scene->list.list[i]);
    }
    free(scene);
}

void addHittable(Scene* scene, Hittable* hittable)
{
    addHittableList(&scene->list, hittable);
}

void sceneAddModel(Scene* scene, Model* model)
{
    scene->models[scene->modelCount++] = *model;
}

void buildSceneBVH(Scene* scene)
{
    //scene->bvh = createBVH(&scene->list, &scene->triangleList);
}

// Try to hit any of the hittable objects in the scene
bool hitScene(Scene* scene, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    HitRecord tempRecord;
    bool hitAnything = false;
    double closestSoFar = 1e30;
    for (int i = 0; i < scene->modelCount; i++)
        if (rayBVHTraversal(&scene->models[i].bvh, ray, rayMin, closestSoFar, &tempRecord))
        {
            hitAnything = true;
            closestSoFar = record->t;
            *record = tempRecord;
            record->material = scene->models[i].material;
        }
    return hitAnything;
}
