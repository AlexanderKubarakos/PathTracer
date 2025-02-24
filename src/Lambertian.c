#include "Lambertian.h"
#include "Vec3.h"

bool scatterLambertian(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered)
{
    Lambertian* lambertian = (Lambertian*)this;
    Vec3 scatterDirection = vec3Add(record->normal, randomUnitVector());

    if (nearZero(scatterDirection))
        scatterDirection = record->normal;

    rayScattered->origin = record->p; 
    rayScattered->direction = scatterDirection;
    rayScattered->invDirection = (Vec3){1/rayScattered->direction.x, 1/rayScattered->direction.y, 1/rayScattered->direction.z};
    *attenuation = lambertian->albedo;
    return true;
};

Lambertian* createLambertian(Color albedo)
{
    Lambertian* mat = malloc(sizeof(Lambertian));
    mat->material = createDefaultMaterial();
    mat->material.scatter = scatterLambertian;
    mat->material.destroy = destoryLambertian;
    mat->albedo = albedo;
    return mat;
}

void destoryLambertian(void* lambertian)
{
    free(lambertian);
}
