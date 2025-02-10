#pragma once
#include "Vec3.h"
#include "Ray.h"
#include "Hittable.h"
typedef struct 
{
    Vec3 a, b, c;
} Triangle;

// Test if we should take in data by pointer here
static inline bool triangleRay(Ray ray, Triangle tri, double rayMin, double rayMax, HitRecord* record)
{
    ray.direction = unitVector(ray.direction); // Is this needed?
    Vec3 edgeAB = subVec3(tri.b, tri.a);
    Vec3 edgeAC = subVec3(tri.c, tri.a);
    Vec3 normal = cross(edgeAB, edgeAC);
    Vec3 ao = subVec3(ray.origin, tri.a);
    Vec3 dao = cross(ao, ray.direction);

    float det = -dot(ray.direction, normal);
    float invDet = 1.0f / det;

    float dst = dot(ao, normal) * invDet;
    float u = dot(edgeAC, dao) * invDet;
    float v = -dot(edgeAB, dao) * invDet;
    float w = 1 - u - v;

    if (dst <= rayMin || rayMax <= dst || !(fabs(det) >= 1E-6 && dst >= 0 && u >= 0 && v >= 0 && w >= 0))
        return false;

    record->t = dst;
    record->p = rayAt(ray, dst);
    // BUG: normal isn't right so sometimes ray bounces on the inside
    Vec3 outwardNormal = unitVector(normal); // THIS MIGHT BE SLOW SINCE WE DO A SQRT!!
    // THIS NORMAL ISN'T CORRECT?
    setFrontFace(record, ray, outwardNormal);
    
    return true;
}