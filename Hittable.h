#pragma once
#include "Ray.h"
#include "Vec3.h"
#include <stdbool.h>

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
    bool (*hitfunc)(void* this, const Ray r, double rayMin, double rayMax, HitRecord* record);
    void (*destroy)(void* this);
    Material* material; // Reference to a material (Not ownership)
} Hittable;

bool hit(Hittable* hittable, const Ray ray, double rayMin, double rayMax, HitRecord* record);
void setFrontFace(HitRecord* record, const Ray ray, const Vec3 outwardNormal); // Outward normal expected to be unit length
//void destroyHitable(Hittable* hittable); // Don't think this needs to exist