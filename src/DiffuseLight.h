#pragma once
#include "Material.h"

typedef struct
{
    Material material;
    Color color;
} DiffuseLight;

DiffuseLight* createDiffuseLight(Color color);
void destoryDiffuseLight(void* diffuseLight);
