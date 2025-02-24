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
    s->triangleList = triangleListCreate(16);
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
    for (int i = 0; i < model->triangleCount; i++)
    {
        triangleListAdd(&scene->triangleList, model->triangles[i]);
    }
}

void buildSceneBVH(Scene* scene)
{
    scene->bvh = createBVH(&scene->list, &scene->triangleList);
}

// Try to hit any of the hittable objects in the scene
bool hitScene(Scene* scene, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    return rayBVHTraversal(&scene->bvh, ray, rayMin, rayMax, record);
}
