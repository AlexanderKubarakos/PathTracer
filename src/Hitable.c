#include "Hittable.h"
#include "Vec3.h"
#include <float.h>
#include <stdlib.h>

bool hit(Hittable* hittable, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    return hittable->hitfunc(hittable, ray, rayMin, rayMax, record);
}

// Outward normal expected to be unit length
void setFrontFace(HitRecord* record, const Ray ray, const Vec3 outwardNormal)
{
    record->frontFace = dot(ray.direction, outwardNormal) < 0;
    record->normal = record->frontFace ? outwardNormal : negVec3(outwardNormal);
}

/*
void destroyHitable(Hittable* hittable)
{
    free(hittable);
}
*/