#pragma once
#include "Vec3.h"

typedef struct
{
    Vec3 origin;
    Vec3 direction;
    Vec3 invDirection;
} Ray;

Vec3 rayAt(const Ray r, const double t);