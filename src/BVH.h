#pragma once

#include "Hittable.h"
#include "AABB.h"

typedef struct BVHNode BVHNode;

struct BVHNode 
{
    AABB boundingBox;
    HittableList hittableList;
    BVHNode* leftChild;
    BVHNode* rightChild;
};

BVHNode createBVH(Hittable** list, int length);