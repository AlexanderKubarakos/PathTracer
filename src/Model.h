#pragma once

// Model.h
// Loads .obj files into Model structs
// Each model has a list of its triangles and a bounding box
// Also contains data for its material in Hittable

#include "AABB.h"
#include "BVH.h"

// TEMP
#include "Hittable.h"
#include "Lambertian.h"

typedef struct
{
    AABB boundingBox;
    BVHNode bvh;
    Material* material;
} Model;

Model* loadOBJModel(const char* fileLocation);
void defaultCube(Model* model);
void deleteModel(Model* model);