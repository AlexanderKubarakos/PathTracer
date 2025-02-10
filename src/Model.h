#pragma once
#include "Triangle.h"
#include "AABB.h"

// TEMP
#include "Hittable.h"
#include "Lambertian.h"

typedef struct
{
    Hittable h;
    AABB boundingBox;
    int triangleCount;
    Triangle* triangles;
} Model;

Model* loadOBJModel(const char* fileLocation);
void defaultCube(Model* model);
void freeModel(Model* model);