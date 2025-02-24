#pragma once
#include "Ray.h"
#include "Vec3.h"
#include <stdbool.h>
#include "AABB.h"
typedef struct Material Material;

typedef struct
{
    Vec3 p;
    Vec3 normal;
    Material* material;
    double t;
    bool frontFace;
} HitRecord;

typedef struct 
{
    // BUG: TODO: add a destroyer function here, right now destory hitable only un allocates the hit func ptr and material ptr, but not the other elements of a hittable object
    // It does clear things part of the block, but not nested pointers
    bool (*hitfunc)(void* shape, const Ray r, double rayMin, double rayMax, HitRecord* record);
    void (*destroy)(void* shape);
    Material* material; // Reference to a material (Not ownership)
    AABB aabb;
} Hittable;

typedef struct
{
    Hittable** list;
    int size;
    int internalLength;
} HittableList;

HittableList createHittableList();
void addHittableList(HittableList* hittableList, Hittable* hittable);

bool hit(Hittable* hittable, const Ray ray, double rayMin, double rayMax, HitRecord* record);
void setFrontFace(HitRecord* record, const Ray ray, const Vec3 outwardNormal); // Outward normal expected to be unit length
//void destroyHitable(Hittable* hittable); // Don't think this needs to exist

// Test if we should take in data by pointer here
static inline bool triangleRay(Ray ray, Triangle tri, double rayMin, double rayMax, HitRecord* record)
{
    ray.direction = unitVector(ray.direction); // Is this needed?
    Vec3 edgeAB = vec3Sub(tri.b, tri.a);
    Vec3 edgeAC = vec3Sub(tri.c, tri.a);
    Vec3 normal = cross(edgeAB, edgeAC);
    Vec3 ao = vec3Sub(ray.origin, tri.a);
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