#pragma once

#include "Vec3.h"
#include "Ray.h"
#include "KubsMath.h"

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
    return (Vec3){aabb->max.x - aabb->min.x, aabb->max.y - aabb->min.y, aabb->max.z - aabb->min.z};
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