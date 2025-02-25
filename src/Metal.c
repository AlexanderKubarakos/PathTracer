#include "Metal.h"
#include "Vec3.h"

bool scatterMetal(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered)
{
    Metal* metal = (Metal*)this;
    Vec3 reflected = reflect(rayIn.direction, record->normal);

    reflected = unitVector(reflected);
    reflected = vec3Add(reflected, vec3Mul(randomUnitVector(), metal->fuzz));

    rayScattered->origin = record->p; 
    rayScattered->direction = reflected;
    rayScattered->invDirection = (Vec3){1/rayScattered->direction.x, 1/rayScattered->direction.y, 1/rayScattered->direction.z};
    *attenuation = metal->albedo; // we could do a reference here and not a copy... I dont know about perfromance
    return dot(reflected, record->normal) > 0;
};

Metal* createMetal(Color albedo, double fuzz)
{
    Metal* mat = malloc(sizeof(Metal));
    mat->material = createDefaultMaterial();
    mat->material.scatter = scatterMetal;
    mat->material.destroy = destoryMetal;
    mat->albedo = albedo;
    mat->fuzz = fuzz;
    return mat;
}

void destoryMetal(void* metal)
{
    free(metal);
}
