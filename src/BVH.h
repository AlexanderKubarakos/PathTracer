#pragma once

#include "Hittable.h"
#include "AABB.h"
#include "Triangle.h"

typedef struct BVHNode BVHNode;

struct BVHNode 
{
    AABB boundingBox;
    HittableList hittableList;
    TriangleList triangleList;
    BVHNode* leftChild;
    BVHNode* rightChild;
};

BVHNode createBVH(HittableList* hittableList, TriangleList* triangleList);
bool rayBVHTraversal(BVHNode* node, const Ray ray, double rayMin, double rayMax, HitRecord* record);