#pragma once
#include "Hittable.h"

// A scene is a list of hitable objects
typedef struct
{
    Hittable** hittableList;
    int length;
    int index;
} Scene;

Scene* createScene(int length);
void destroyScene(Scene* scene);
void addHittable(Scene* scene, Hittable* hittable);
bool hitScene(Scene* scene, const Ray ray, double rayMin, double rayMax, HitRecord* record);