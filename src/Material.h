#pragma once
#include "stdbool.h"
#include "Ray.h"
#include "Hittable.h"
#include "PPM.h"
typedef struct Material Material;
struct Material
{
    bool (*scatter)(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered);
    Color (*emission)(void* this);
    void (*destroy)(void* this);
};

// Create a default material that comes with default func ptrs, so that you dont have to overload them all... (mainly emssive)
Material createDefaultMaterial();
void destroyMaterial(Material* this);
