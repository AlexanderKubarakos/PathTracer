#pragma once
#include "Hittable.h"
#include "Material.h"

typedef struct
{
    Material material;
    Color albedo;
} Lambertian;

Lambertian* createLambertian(Color albedo);
bool scatterLambertian(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered);
void destoryLambertian(void* lambertian);