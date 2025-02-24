#pragma once
#include "Vec3.h"

typedef struct 
{
    Vec3 a, b, c;
} Triangle;

float triangleCenter(Triangle tri, int axis);

typedef struct
{
    int length;
    int capacity;
    Triangle* list;
} TriangleList;

TriangleList triangleListCreate(int initalSize);
void triangleListAdd(TriangleList* list, Triangle tri);
void triangleListDestroy(TriangleList* list);
// TODO: destruction function

#define iterateTriangleList(listLength, itr) for (int (itr) = 0; (itr) < (listLength); (itr)++)