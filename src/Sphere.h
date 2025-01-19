#pragma once
#include "Hittable.h"
#include <stdbool.h>

typedef struct
{
    Hittable h;
    Vec3 center;
    double radius;
} Sphere;

bool hitSphere(void* this, const Ray r, double rayMin, double rayMax, HitRecord* record);
Hittable* createSphere(const Vec3 center, const double radius, Material* m);
void destroySphere(void* this);