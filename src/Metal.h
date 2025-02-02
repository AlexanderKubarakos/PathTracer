#pragma once
#include "Material.h"

typedef struct
{
    Material material;
    Color albedo;
    double fuzz;
} Metal;

Metal* createMetal(Color albedo, double fuzz);
void destoryMetal(void* metal);
