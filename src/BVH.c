#include "BVH.h"
#include "AABB.h"
#include "Lambertian.h"
#include "Triangle.h"
#include <stdio.h>
#include <stdlib.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

const static int maxDepth = 32;
static int maxTriangle = 0;
static int minTriangle = 1000000000;
static int minNodeDepth = 100000000;
static int maxNodeDepth = 0;

float nodeCost(Vec3 size, int num)
{
    float halfArea = size.x * (size.y + size.z) + size.y * size.z;
    return halfArea * num;
}

float evaluteSplit(BVHNode* node, int axis, float pos)
{
    AABB a = createAABB((Vec3){1e30f, 1e30f, 1e30f}, (Vec3){-1e30f,-1e30f,-1e30f});
    AABB b = createAABB((Vec3){1e30f,1e30f,1e30f}, (Vec3){-1e30f,-1e30f,-1e30f});
    int countA = 0;
    int countB = 0;

    // TODO: update for spherees

    iterateTriangleList(node->triangleList.length, i)
    {
        Triangle* tri = node->triangleList.list + i;
        if (triangleCenter(*tri, axis) < pos)
        {
            a = expandAABBTriangle(a, *tri);
            countA++;
        }
        else 
        {
            b = expandAABBTriangle(b, *tri);
            countB++;
        }
    } 

    float cost = countA * areaAABB(a) + countB * areaAABB(b);
    return cost > 0 ? cost : 1e30f;
}

void chooseSplit(BVHNode* parent, int* retSplitAxis, float* retSplitPos, float* retCost)
{
    float bestCost = 1e30f;
    float bestPos = 0;
    int bestAxis = 0;

    for (int axis = 0; axis < 3; axis++)
    {
        float boundsStart;
        float boundsEnds;
        switch (axis)
        {
            case 0:
                boundsStart = parent->boundingBox.min.x;
                boundsEnds = parent->boundingBox.max.x;
                break;
            case 1:
                boundsStart = parent->boundingBox.min.y;
                boundsEnds = parent->boundingBox.max.y;
            break;
            case 2:
                boundsStart = parent->boundingBox.min.z;
                boundsEnds = parent->boundingBox.max.z;
            break;
        }

        int splitChecks = 10;

        for (int i = 0; i < splitChecks; i++)
        {
            float splitT = (i+1) / (splitChecks + 1.0f);
            float pos = boundsStart + (boundsEnds - boundsStart) * splitT;
            float cost = evaluteSplit(parent, axis, pos);

            if (cost < bestCost)
            {
                bestCost = cost;
                bestPos = pos;
                bestAxis = axis;
            }
        }
    }

    *retSplitAxis = bestAxis;
    *retCost = bestCost;
    *retSplitPos = bestPos;
    return;
}

void split(BVHNode* parent, int depth)
{
    if (depth == maxDepth)
    {
        printf("MAX DEPTH: I am a leaf with length: %i\n", parent->hittableList.size);
        return;
    }

    parent->leftChild = calloc(1, sizeof(BVHNode));
    parent->rightChild = calloc(1, sizeof(BVHNode));
    parent->leftChild->hittableList = createHittableList();
    parent->rightChild->hittableList = createHittableList();

    triangleListCreate(&parent->leftChild->triangleList, 16);
    triangleListCreate(&parent->rightChild->triangleList, 16);

    int axis;
    float splitPos;
    float cost;
    chooseSplit(parent, &axis, &splitPos, &cost);

    // Calculate parents cost
    float parentArea = areaAABB(parent->boundingBox);
    float parentCost = parent->triangleList.length * parentArea;

    if (cost >= parentCost) 
    {
        minTriangle = min(minTriangle, parent->triangleList.length);
        maxTriangle = max(maxTriangle, parent->triangleList.length);
        minNodeDepth = min(minNodeDepth, depth);
        maxNodeDepth = max(maxNodeDepth, depth);
        parent->leftChild = NULL;
        parent->rightChild = NULL;
        return;
    }
    // Split for Hittables
    for (int i = 0; i < parent->hittableList.size; i++)
    {
        int whatChild = centerAxis(&parent->hittableList.list[i]->aabb, axis) < splitPos;
        BVHNode* child = whatChild ? parent->leftChild : parent->rightChild;
        addHittableList(&child->hittableList, parent->hittableList.list[i]);
        child->boundingBox = expandAABB(child->boundingBox, parent->hittableList.list[i]->aabb);
    }

    //Split for Triangles
    iterateTriangleList(parent->triangleList.length, i)
    {
        int whatChild = triangleCenter(parent->triangleList.list[i], axis) < splitPos;
        BVHNode* child = whatChild ? parent->leftChild : parent->rightChild;
        triangleListAdd(&child->triangleList, parent->triangleList.list[i]);
        child->boundingBox = expandAABBTriangle(child->boundingBox, parent->triangleList.list[i]);
    }

    if ((parent->leftChild->hittableList.size > 0 || parent->leftChild->triangleList.length > 0)
        && (parent->rightChild->hittableList.size > 0 || parent->rightChild->triangleList.length > 0))
    {
        split(parent->leftChild, depth+1);
        split(parent->rightChild, depth+1);
    }
    else
    {
        //printf("NO SPLIT: I am a leaf with: %i Hittables and %i Triangles, my split axis was: %i, my depth was %i\n", parent->hittableList.size, parent->triangleList.length, axis, depth);
        
        //AABB aabb = parent->boundingBox;
        //printf("AABB bounds min(%f, %f, %f), max(%f, %f, %f)\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
        // TODO: delete children
        // AKA, make our self a leaf
        parent->leftChild = NULL;
        parent->rightChild = NULL;
    }   
}

static Lambertian* mat;

BVHNode createBVH(HittableList* hittableList, TriangleList* triangleList)
{
    // TODO REMOVE
    mat = createLambertian((Color){0.2,0.2,0.2});
    if (hittableList->size == 0 && triangleList->length == 0)
        return (BVHNode){};
    BVHNode node;
    node.hittableList = *hittableList;
    node.triangleList = *triangleList;

    AABB aabb = (AABB){(Vec3){0,0,0}, (Vec3){0,0,0}};

    // Handle Hittables
    for (int i = 0; i < hittableList->size; i++)
    {
        aabb = expandAABB(aabb, hittableList->list[i]->aabb);
    }

    // Handle Triangles
    iterateTriangleList(triangleList->length, i)
    {
        aabb = expandAABBTriangle(aabb, triangleList->list[i]);
    }

    node.boundingBox = aabb;
    node.leftChild = NULL;
    node.rightChild = NULL;
    split(&node,0);
    printf("BVH Stats - Min Tri: %i - Max Tri %i - Min depth %i - Max depth %i\n", minTriangle, maxTriangle, minNodeDepth, maxNodeDepth);
    return node;
}

bool rayBVHTraversal(BVHNode* node, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    HitRecord tempRecord;
    bool hitAnything = false;
    double closestSoFar = rayMax;

    bool boxHit = rayAABBIntersection(node->boundingBox, ray, rayMax);
    if (!boxHit)
        return false;

    if (node->leftChild == NULL && node->rightChild == NULL)
    {
        for (int i = 0; i < node->hittableList.size; i++)
        {
            if(hit(node->hittableList.list[i], ray, rayMin, closestSoFar, &tempRecord))
            {
                hitAnything = true;
                closestSoFar = tempRecord.t;
                *record = tempRecord;
            }
        }
        
        iterateTriangleList(node->triangleList.length, i)
        {
            if (triangleRay(ray, node->triangleList.list[i], rayMin, closestSoFar, &tempRecord))
            {
                hitAnything = true;
                closestSoFar = tempRecord.t;
                *record = tempRecord;
                record->material = (Material*)mat;
            }
        }
    }
    else
    {
        if(rayBVHTraversal(node->leftChild, ray, rayMin, rayMax, record))
        {
            hitAnything = true;
            closestSoFar = record->t;
        }
        if(rayBVHTraversal(node->rightChild, ray, rayMin, closestSoFar, record))
        {
            hitAnything = true;
        }
    }

    return hitAnything;
}