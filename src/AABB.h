#pragma once

#include "Vec3.h"
#include "Ray.h"
#include "KubsMath.h"
#include "Triangle.h"
#include <immintrin.h>
typedef struct 
{
    union {
        struct {Vec3 min; int dummy1;};
        __m128 minIntrinsic;
    };
    
    union {
        struct {Vec3 max; int dummy2;};
        __m128 maxIntrinsic;
    };
} AABB;

static float center(float min, float max)
{
    return min + (max-min)*0.5f;
}

static inline Vec3 aabbBoundsSize(AABB* aabb)
{
    return vec3Sub(aabb->max, aabb->min);
}

static inline float centerAxis(AABB* aabb, int axis)
{
    if (axis == 1)
    {
        // y
        return center(aabb->min.y, aabb->max.y);
    } else if (axis == 2)
    {
        //z
        return center(aabb->min.z, aabb->max.z);
    }

    return center(aabb->min.x, aabb->max.x);
}

static inline AABB createAABBEmpty()
{
    return (AABB){.min=(Vec3){1e30f, 1e30f, 1e30f}, .max=(Vec3){-1e30f, -1e30f, -1e30f}};
}

static inline AABB createAABB(Vec3 min, Vec3 max)
{
    return (AABB){.min = min, .max = max};
}

static inline double rayAABBIntersection(const AABB box, const Ray ray, const double t)
{
    __m128 t1 = _mm_mul_ps(_mm_sub_ps(box.minIntrinsic,ray.O),ray.ID );
    __m128 t2 = _mm_mul_ps(_mm_sub_ps(box.maxIntrinsic,ray.O),ray.ID );
    __m128 vmax4 = _mm_max_ps(t1,t2);
    __m128 vmin4 = _mm_min_ps(t1,t2);
    float tmax = minFloat(vmax4[0],minFloat(vmax4[1],vmax4[2]));
    float tmin = maxFloat(vmin4[0],maxFloat(vmin4[1],vmin4[2]));

    if (tmax >= maxFloat(0.0, tmin) && tmin < t)
        return tmin;
    return 1e30;
} 

static inline AABB expandAABB(AABB a, AABB b)
{
    AABB c;
    c.min = (Vec3){minFloat(a.min.x, b.min.x), minFloat(a.min.y, b.min.y), minFloat(a.min.z, b.min.z)};
    c.max = (Vec3){maxFloat(a.max.x, b.max.x), maxFloat(a.max.y, b.max.y), maxFloat(a.max.z, b.max.z)};
    return c;
}

static inline AABB growToIncludePoint(AABB a, Vec3 p)
{
    AABB b;
    b.min = (Vec3){minFloat(a.min.x, p.x), minFloat(a.min.y, p.y), minFloat(a.min.z, p.z)};
    b.max = (Vec3){maxFloat(a.max.x, p.x), maxFloat(a.max.y, p.y), maxFloat(a.max.z, p.z)};
    return b;
}

static inline AABB expandAABBTriangle(AABB a, Triangle triangle)
{
    a = growToIncludePoint(a, triangle.a);
    a = growToIncludePoint(a, triangle.b);
    a = growToIncludePoint(a, triangle.c);
    return a;
}

static inline double areaAABB(AABB a)
{
    Vec3 bounds = aabbBoundsSize(&a);
    return bounds.x * bounds.y + bounds.y * bounds.z + bounds.x * bounds.z;
}