#include "Metal.h"
#include "Vec3.h"

bool scatterMetal(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered)
{
    Metal* metal = (Metal*)this;
    Vec3 reflected = reflect(rayIn.direction, record->normal);

    reflected = unitVector(reflected);
    reflected = addVec3(reflected, mulVec3(randomUnitVector(), metal->fuzz));

    rayScattered->origin = record->p; 
    rayScattered->direction = reflected;
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
