#pragma once
#include "Material.h"

typedef struct
{
    Material material;
    double refractionIndex;
} Dielectric;

Dielectric* createDielectric(double refractionIndex);
bool scatterDielectric(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered);
void destoryDielectric(void* dielectric);