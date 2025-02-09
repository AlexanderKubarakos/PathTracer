#pragma once
#include "Hittable.h"
#include "BVH.h"
// A scene is a list of hitable objects
typedef struct
{
    HittableList list;
    BVHNode bvh;
} Scene;

Scene* createScene(int length);
void destroyScene(Scene* scene);
void addHittable(Scene* scene, Hittable* hittable);
void buildSceneBVH(Scene* scene);
bool hitScene(Scene* scene, const Ray ray, double rayMin, double rayMax, HitRecord* record);