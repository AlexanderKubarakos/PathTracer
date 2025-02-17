#include "Triangle.h"
#define ALIGN(x, n)                  ((x) + (n)) & ~((n) - 1)

float triangleCenter(Triangle tri, int axis)
{
    if (axis == 0)
    {
        return (tri.a.x + tri.b.x + tri.c.x) / 3.0;
    } else if (axis == 1)
    {
        return (tri.a.y + tri.b.y + tri.c.y) / 3.0;
    }

    return (tri.a.z + tri.b.z + tri.c.z) / 3.0;
}

void triangleListCreate(TriangleList* list, int initalSize)
{
    if (initalSize < 0)
        return;
    list->capacity = initalSize;
    list->length = 0;
    list->list = malloc(sizeof(Triangle) * list->capacity);
}

void triangleListAdd(TriangleList* list, Triangle tri)
{
    if (list->length + 1 >= list->capacity)
    {
        list->list = realloc(list->list, sizeof(Triangle) * list->capacity*2);
        list->capacity = list->capacity * 2;
    }
    list->list[list->length] = tri;
    list->length++;
}