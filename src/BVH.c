#include "BVH.h"

const static int maxDepth = 10;

void split(BVHNode* parent, int depth)
{
    if (depth == maxDepth)
        return;

    parent->leftChild = calloc(1, sizeof(BVHNode));
    parent->rightChild = calloc(1, sizeof(BVHNode));
    parent->leftChild->hittableList = createHittableList();
    parent->rightChild->hittableList = createHittableList();
    for (int i = 0; i < parent->hittableList.size; i++)
    {
        int whatChild = parent->hittableList.list[i]->aabb.max.x - parent->hittableList.list[i]->aabb.min.x <  parent->boundingBox.max.x - parent->boundingBox.min.x;
        BVHNode* child = whatChild ? parent->leftChild : parent->rightChild;
        addHittableList(&child->hittableList, parent->hittableList.list[i]);
        child->boundingBox = expandAABB(child->boundingBox, parent->hittableList.list[i]->aabb);
    }

    split(parent->leftChild, depth+1);
    split(parent->rightChild, depth+1);
};

BVHNode createBVH(Hittable* list, int length)
{
    if (length == 0)
        return (BVHNode){};
    BVHNode node;
    node.hittableList = createHittableList();

    AABB aabb = (AABB){(Vec3){0,0,0}, (Vec3){0,0,0}};
    for (int i = 0; i < length; i++)
    {
        aabb = expandAABB(aabb, list[i].aabb);
        addHittableList(&node.hittableList, &list[i]);
    }

    node.boundingBox = aabb;

    split(&node,0);
    return node;
}