#pragma once

#include "Vec3.h"
#include "Ray.h"
#include "KubsMath.h"
#include "Triangle.h"

typedef struct 
{
    Vec3 min;
    Vec3 max;
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
    return (AABB){(Vec3){1e30f, 1e30f, 1e30f}, (Vec3){-1e30f, -1e30f, -1e30f}};
}

static inline AABB createAABB(Vec3 min, Vec3 max)
{
    return (AABB){.min = min, .max = max};
}

static inline bool rayAABBIntersection(const AABB box, const Ray ray, const double t)
{
    double tx1 = (box.min.x - ray.origin.x)*ray.invDirection.x;
    double tx2 = (box.max.x - ray.origin.x)*ray.invDirection.x;

    double tmin = minFloat(tx1, tx2);
    double tmax = maxFloat(tx1, tx2);

    double ty1 = (box.min.y - ray.origin.y)*ray.invDirection.y;
    double ty2 = (box.max.y - ray.origin.y)*ray.invDirection.y;

    tmin = maxFloat(tmin, minFloat(ty1, ty2));
    tmax = minFloat(tmax, maxFloat(ty1, ty2));

    double tz1 = (box.min.z - ray.origin.z)*ray.invDirection.z;
    double tz2 = (box.max.z - ray.origin.z)*ray.invDirection.z;

    tmin = maxFloat(tmin, minFloat(tz1, tz2));
    tmax = minFloat(tmax, maxFloat(tz1, tz2));

    return tmax >= maxFloat(0.0, tmin) && tmin < t;
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