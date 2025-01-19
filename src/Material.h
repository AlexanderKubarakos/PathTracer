#pragma once
#include "stdbool.h"
#include "Ray.h"
#include "Hittable.h"
#include "PPM.h"
typedef struct Material Material;
struct Material
{
    bool (*scatter)(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered);
    void (*destroy)(void* this);
};

void destroyMaterial(Material* this);