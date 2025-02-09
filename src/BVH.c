#include "BVH.h"
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

    Vec3 bounds = aabbBoundsSize(&parent->boundingBox);
    int splitAxis = bounds.x > maxFloat(bounds.y, bounds.z) ? 0 : bounds.y > bounds.z ? 1 : 2;
    float splitPos = centerAxis(&parent->boundingBox, splitAxis);
    for (int i = 0; i < parent->hittableList.size; i++)
    {
        int whatChild = centerAxis(&parent->hittableList.list[i]->aabb, splitAxis) < splitPos;
        BVHNode* child = whatChild ? parent->leftChild : parent->rightChild;
        addHittableList(&child->hittableList, parent->hittableList.list[i]);
        child->boundingBox = expandAABB(child->boundingBox, parent->hittableList.list[i]->aabb);
    }

    if (parent->leftChild->hittableList.size > 0 && parent->rightChild->hittableList.size > 0)
    {
        split(parent->leftChild, depth+1);
        split(parent->rightChild, depth+1);
    }
    else
    {
        printf("NO SPLIT: I am a leaf with length: %i, my split was: %i, my depth was %i\n", parent->hittableList.size, splitAxis, depth);
        // TODO: delete children
        parent->leftChild = NULL;
        parent->rightChild = NULL;
    }   
}

BVHNode createBVH(Hittable** list, int length)
{
    if (length == 0)
        return (BVHNode){};
    BVHNode node;
    node.hittableList = createHittableList();

    AABB aabb = (AABB){(Vec3){0,0,0}, (Vec3){0,0,0}};
    for (int i = 0; i < length; i++)
    {
        aabb = expandAABB(aabb, list[i]->aabb);
        addHittableList(&node.hittableList, list[i]);
    }

    node.boundingBox = aabb;
    node.leftChild = NULL;
    node.rightChild = NULL;
    printf("AABB bounds min(%f, %f, %f), max(%f, %f, %f)\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
    split(&node,0);
    return node;
}