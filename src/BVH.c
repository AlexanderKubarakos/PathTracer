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

BVHNode createBVHNode()
{
    BVHNode node;
    node.boundingBox = createAABBEmpty();
    node.triangleList = triangleListCreate(1);
    node.leftChild = NULL;
    node.rightChild = NULL;
    return node;
}

void deleteBVHNode(BVHNode* node)
{
    // Add triangle list destruction here
    triangleListDestroy(&node->triangleList);
}

float nodeCost(Vec3 size, int num)
{
    float halfArea = size.x * (size.y + size.z) + size.y * size.z;
    return halfArea * num;
}

float evaluateSplit(BVHNode* node, int axis, float pos)
{
    AABB a = createAABB((Vec3){1e30f, 1e30f, 1e30f}, (Vec3){-1e30f, -1e30f, -1e30f});
    AABB b = createAABB((Vec3){1e30f, 1e30f, 1e30f}, (Vec3){-1e30f, -1e30f, -1e30f});
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

// Choose how we should split our box, returns the axis, the position along that axis, and the best cost we could find
// TODO: Should we try to split along all 3 axis for each triangle?
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

        int splitChecks = 300;

        for (int i = 0; i < splitChecks; i++)
        {
            float splitT = (i+1) / (splitChecks + 1.0f);
            float pos = boundsStart + (boundsEnds - boundsStart) * splitT;
            float cost = evaluateSplit(parent, axis, pos);

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
        // We are forced to be a leaf here, since we have hit max depth
        parent->leftChild = NULL;
        parent->rightChild = NULL;
        return;
    }

    parent->leftChild = malloc(sizeof(BVHNode));
    *parent->leftChild  = createBVHNode();
    parent->rightChild = malloc(sizeof(BVHNode));
    *parent->rightChild  = createBVHNode();

    int axis;
    float splitPos;
    float cost;
    chooseSplit(parent, &axis, &splitPos, &cost);

    parent->splitAxis = axis;

    // Calculate parents cost
    float parentArea = areaAABB(parent->boundingBox);
    float parentCost = parent->triangleList.length * parentArea;

    if (cost >= parentCost) 
    {
        // Don't split! This is now a leaf
        minTriangle = min(minTriangle, parent->triangleList.length);
        maxTriangle = max(maxTriangle, parent->triangleList.length);
        minNodeDepth = min(minNodeDepth, depth);
        maxNodeDepth = max(maxNodeDepth, depth);
        // BUG: TODO: CALL DESTROY NODE
        parent->leftChild = NULL;
        parent->rightChild = NULL;
        return;
    }

    // It is better to split, so split into children

    //Split for Triangles
    iterateTriangleList(parent->triangleList.length, i)
    {
        int whatChild = triangleCenter(parent->triangleList.list[i], axis) < splitPos;
        BVHNode* child = whatChild ? parent->leftChild : parent->rightChild;
        triangleListAdd(&child->triangleList, parent->triangleList.list[i]);
        child->boundingBox = expandAABBTriangle(child->boundingBox, parent->triangleList.list[i]);
    }

    // Finally recursivly split our children

    split(parent->leftChild, depth+1);
    split(parent->rightChild, depth+1);
    return;
}

BVHNode createBVH(TriangleList* triangleList)
{
    _mm_setcsr(_mm_getcsr() | (_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON));

    if (triangleList->length == 0)
        return (BVHNode){};

    AABB aabb = createAABB((Vec3){1e30f, 1e30f, 1e30f}, (Vec3){-1e30f, -1e30f, -1e30f});

    // Handle Triangles
    iterateTriangleList(triangleList->length, i)
    {
        aabb = expandAABBTriangle(aabb, triangleList->list[i]);
    }

    BVHNode node = createBVHNode();
    node.triangleList = *triangleList;
    node.boundingBox = aabb;
    
    split(&node,0);
    printf("BVH Stats - Min Tri: %i - Max Tri %i - Min depth %i - Max depth %i\n", minTriangle, maxTriangle, minNodeDepth, maxNodeDepth);
    printf("Bounds: "); vec3Print(node.boundingBox.min); printf(" "); vec3Print(node.boundingBox.max); printf("\n");
    return node;
}

bool rayBVHTraversal(BVHNode* node, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    HitRecord tempRecord;
    bool hitAnything = false;
    double closestSoFar = rayMax;

    double boxHit = rayAABBIntersection(node->boundingBox, ray, rayMax);
    if (boxHit == 1e30)
        return false;

    if (node->leftChild == NULL && node->rightChild == NULL)
    {
        iterateTriangleList(node->triangleList.length, i)
        {
            if (triangleRay(ray, node->triangleList.list[i], rayMin, closestSoFar, &tempRecord))
            {
                hitAnything = true;
                closestSoFar = tempRecord.t;
                *record = tempRecord;
            }
        }
    }
    else
    {
        bool dirNeg[3] = {ray.direction.x < 0,ray.direction.y < 0,ray.direction.z < 0};
        if(dirNeg[node->splitAxis])
        {
            if(rayBVHTraversal(node->rightChild, ray, rayMin, rayMax, record))
            {
                hitAnything = true;
                closestSoFar = record->t;
            }
            if(rayBVHTraversal(node->leftChild, ray, rayMin, closestSoFar, record))
            {
                hitAnything = true;
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
    }

    return hitAnything;
}