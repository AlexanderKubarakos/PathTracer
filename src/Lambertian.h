#pragma once
#include "Hittable.h"
#include "Material.h"

typedef struct
{
    Material material;
    Color albedo;
} Lambertian;

Lambertian* createLambertian(Color albedo);
void destoryLambertian(void* lambertian);
