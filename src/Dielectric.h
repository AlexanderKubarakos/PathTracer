#pragma once
#include "Material.h"

typedef struct
{
    Material material;
    double refractionIndex;
} Dielectric;

Dielectric* createDielectric(double refractionIndex);
void destoryDielectric(void* dielectric);
