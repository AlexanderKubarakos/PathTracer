#include "Sphere.h"
#include "Hittable.h"
#include "Ray.h"
#include "Vec3.h"
#include <math.h>
#include <stdlib.h>

Hittable* createSphere(const Vec3 center, const double radius, Material* m)
{
    Sphere* sphere = malloc(sizeof(Sphere));
    sphere->h.hitfunc = hitSphere;
    sphere->h.destroy = destroySphere;
    Vec3 radiusVector = (Vec3){radius, radius, radius};
    sphere->h.aabb = createAABB(vec3Sub(center, radiusVector), vec3Add(center, radiusVector));
    sphere->h.material = m; 
    sphere->center = center;
    sphere->radius = radius;
    return (Hittable*)sphere;
}

void destroySphere(void *this)
{
    free(this);
}

bool hitSphere(void* this, const Ray r, double rayMin, double rayMax, HitRecord* record)
{
    Sphere* sphere = (Sphere*)this;
    const Vec3 circleCenter = sphere->center; 
    double radius = sphere->radius;
    Vec3 oc = vec3Sub(circleCenter, r.origin);
    double a = lengthSquared(r.direction);
    double h = dot(r.direction, oc);
    double c = lengthSquared(oc) - radius * radius;
    double discriminant = h*h-a*c;

    if (discriminant < 0)
    {
        return false;
    }

    double sqrtd = sqrt(discriminant);

    double root = (h - sqrtd) / a;
    if (root <= rayMin || rayMax <= root)
    {
        root = (h + sqrtd) / a;
        if (root <= rayMin || rayMax <= root)
            return false;
    }

    record->t = root;
    record->p = rayAt(r, root);
    record->material = sphere->h.material;
    Vec3 outwardNormal = vec3Div(vec3Sub(record->p, circleCenter), radius);
    setFrontFace(record, r, outwardNormal);
    return true;
}
