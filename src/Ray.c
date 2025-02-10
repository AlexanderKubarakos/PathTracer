#include "Ray.h"

Vec3 rayAt(const Ray r, const double t)
{
    return addVec3(r.origin, mulVec3(r.direction, t));
}