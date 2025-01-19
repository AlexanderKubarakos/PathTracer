#pragma once
#include "Material.h"

typedef struct
{
    Material material;
    Color albedo;
    double fuzz;
} Metal;

Metal* createMetal(Color albedo, double fuzz);
bool scatterMetal(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered);
void destoryMetal(void* metal);