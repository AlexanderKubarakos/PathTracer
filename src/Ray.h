#pragma once
#include "Vec3.h"

#include <immintrin.h>

typedef struct
{
    union {struct{Vec3 origin; float dummy1;}; __m128 O;};
    union {struct{Vec3 direction; float dummy2;}; __m128 D;};
    union {struct{Vec3 invDirection; float dummy3;}; __m128 ID;};
} Ray;

Vec3 rayAt(const Ray r, const double t);