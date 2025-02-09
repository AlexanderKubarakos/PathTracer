#include "Scene.h"
#include "Hittable.h"

#include <stdlib.h>
#include <stdio.h>
Scene* createScene(int length)
{
    Scene* s = malloc(sizeof(Scene));
    if (!s)
    {
        printf("Failed to allocate memory for scene\n");
    }

    s->list = createHittableList();

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

void buildSceneBVH(Scene* scene)
{
    scene->bvh = createBVH(scene->list.list, scene->list.size);
}

bool rayBVHTraversal(Scene* scene, BVHNode* node, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    HitRecord tempRecord;
    bool hitAnything = false;
    double closestSoFar = rayMax;

    bool boxHit = rayAABBIntersection(node->boundingBox, ray, 100000000000.0f);
    if (!boxHit)
        return false;

    if (node->leftChild == NULL && node->rightChild == NULL)
    {
        for (int i = 0; i < node->hittableList.size; i++)
        {
            if(hit(node->hittableList.list[i], ray, rayMin, closestSoFar, &tempRecord))
            {
                hitAnything = true;
                closestSoFar = tempRecord.t;
                *record = tempRecord;
            }
        }
    }
    else
    {
        if(rayBVHTraversal(scene, node->leftChild, ray, rayMin, rayMax, record))
        {
            hitAnything = true;
            closestSoFar = record->t;
        }
        if(rayBVHTraversal(scene, node->rightChild, ray, rayMin, closestSoFar, record))
        {
            hitAnything = true;
        }
    }

    return hitAnything;
}

// Try to hit any of the hittable objects in the scene
bool hitScene(Scene* scene, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    #define BVH
#ifdef BVH
    return rayBVHTraversal(scene, &scene->bvh, ray, rayMin, rayMax, record);
#else
    HitRecord tempRecord;
    bool hitAnything = false;
    double closestSoFar = rayMax;
    for (int i = 0; i < scene->list.size; i++)
    {
        if (hit(scene->list.list[i], ray, rayMin, closestSoFar, &tempRecord))
        {
            hitAnything = true;
            closestSoFar = tempRecord.t;
            *record = tempRecord;
        }
    }

    return hitAnything;
#endif
}
