#include "Scene.h"
#include "Hittable.h"

#include <stdlib.h>
#include <stdio.h>
Scene* createScene(int length)
{
    Scene* s = malloc(sizeof(Scene));
    if (!s)
    {
        fprintf(stderr, "Failed to allocate memory for scene\n");
    }

    s->hittableList = calloc(length, sizeof(Hittable*));
    if (!s->hittableList)
    {
        fprintf(stderr, "Failed to allocate memory for scene hittable list\n");
    }

    s->length = length;
    s->index = 0;
    return s;
}

void destroyScene(Scene* scene)
{
    for (int i = 0; i < scene->length; i++)
    {
        if (scene->hittableList[i] == NULL)
            break;
        scene->hittableList[i]->destroy(scene->hittableList[i]);
    }
    free(scene);
}

void addHittable(Scene* scene, Hittable* hittable)
{
    if (scene->index == scene->length)
    {
        // List is full
        fprintf(stderr, "Hittable list full");
        return;
    }

    scene->hittableList[scene->index] = hittable;
    scene->index++;
}

// Try to hit any of the hittable objects in the scene
bool hitScene(Scene* scene, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    HitRecord tempRecord;
    bool hitAnything = false;
    double closestSoFar = rayMax;
    for (int i = 0; i < scene->index; i++)
    {
        if (scene->hittableList[i] == NULL)
            break;
        if (hit(scene->hittableList[i], ray, rayMin, closestSoFar, &tempRecord))
        {
            hitAnything = true;
            closestSoFar = tempRecord.t;
            *record = tempRecord;
        }
    }

    return hitAnything;
}
