#include "BVH.h"
#include "AABB.h"
#include "Lambertian.h"
#include "Triangle.h"
#include <stdio.h>
const static int maxDepth = 32;

// rename this node? not parent?
void split(BVHNode* parent, int depth)
{
    //printf("Spliting a new node, depth = %i\n", depth);
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

    Vec3 bounds = aabbBoundsSize(&parent->boundingBox);
    int splitAxis = bounds.x > maxFloat(bounds.y, bounds.z) ? 0 : bounds.y > bounds.z ? 1 : 2;
    float splitPos = centerAxis(&parent->boundingBox, splitAxis);

    // Split for Hittables
    for (int i = 0; i < parent->hittableList.size; i++)
    {
        int whatChild = centerAxis(&parent->hittableList.list[i]->aabb, splitAxis) < splitPos;
        BVHNode* child = whatChild ? parent->leftChild : parent->rightChild;
        addHittableList(&child->hittableList, parent->hittableList.list[i]);
        child->boundingBox = expandAABB(child->boundingBox, parent->hittableList.list[i]->aabb);
    }

    //Split for Triangles
    iterateTriangleList(parent->triangleList.length, i)
    {
        int whatChild = triangleCenter(parent->triangleList.list[i], splitAxis) < splitPos;
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
        printf("NO SPLIT: I am a leaf with: %i Hittables and %i Triangles, my split axis was: %i, my depth was %i\n", parent->hittableList.size, parent->triangleList.length, splitAxis, depth);
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
    printf("AABB bounds min(%f, %f, %f), max(%f, %f, %f)\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    split(&node,0);
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