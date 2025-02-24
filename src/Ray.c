#include "Ray.h"

Vec3 rayAt(const Ray r, const double t)
{
    return vec3Add(r.origin, vec3Mul(r.direction, t));
}