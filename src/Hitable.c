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
    record->normal = record->frontFace ? outwardNormal : vec3Neg(outwardNormal);
}

HittableList hittableListCreate()
{
    HittableList h;
    h.list = malloc(sizeof(Hittable*) * 16);
    h.size = 0;
    h.internalLength = 16;
    return h;
}

void hittableListDestory(HittableList* list)
{
    free(list->list);
}

void addHittableList(HittableList* hittableList, Hittable* hittable)
{
    if(hittableList->internalLength == hittableList->size+1)
    {
        hittableList->list = realloc(hittableList->list, 2*hittableList->internalLength*sizeof(Hittable*));
        hittableList->internalLength = 2*hittableList->internalLength;
    }

    hittableList->list[hittableList->size] = hittable;
    hittableList->size++;
}

/*
void destroyHitable(Hittable* hittable)
{
    free(hittable);
}
*/