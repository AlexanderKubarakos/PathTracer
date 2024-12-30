#include "Ray.h"

Vec3 rayAt(Ray r, double t)
{
    return addVec3(r.origin, mulVec3(r.direction, t));
}