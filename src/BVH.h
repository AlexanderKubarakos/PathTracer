#pragma once

#include "Hittable.h"
#include "AABB.h"
#include "Triangle.h"

typedef struct BVHNode BVHNode;

struct BVHNode 
{
    AABB boundingBox; 
    HittableList* hittableList; 
    TriangleList triangleList; // 16
    BVHNode* leftChild; // 8
    BVHNode* rightChild; // 8
    int splitAxis;
    // 24 + 16 + 16 + 8 + 8 = 72 // I need to remove 8 bytes for cache line
};

//#define a sizeof(BVHNode)

BVHNode createBVH(HittableList* hittableList, TriangleList* triangleList);
bool rayBVHTraversal(BVHNode* node, const Ray ray, double rayMin, double rayMax, HitRecord* record);