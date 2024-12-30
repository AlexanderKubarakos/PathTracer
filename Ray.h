#pragma once
#include "Vec3.h"

typedef struct
{
    Vec3 origin;
    Vec3 direction;
} Ray;

Vec3 rayAt(Ray r, double t);