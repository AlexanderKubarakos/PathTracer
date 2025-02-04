#include "Dielectric.h"
#include "KubsMath.h"
#include "Ray.h"
#include "Vec3.h"

static double reflectance(double cosine, double refractionIndex) {
    // Use Schlick's approximation for reflectance.
    double r0 = (1 - refractionIndex) / (1 + refractionIndex);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

bool scatterDielectric(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered)
{
    Dielectric* mat = (Dielectric*)this;
    *attenuation = (Color){1.0,1.0,1.0};
    double refractionIndex = record->frontFace ? (1.0/mat->refractionIndex) : mat->refractionIndex;

    Vec3 unitDirection = unitVector(rayIn.direction);
    double cosTheta = fmin(dot(negVec3(unitDirection), record->normal), 1.0);
    double sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    Vec3 direction;

    if (refractionIndex * sinTheta > 1.0 || reflectance(cosTheta, refractionIndex) > randomDouble())
    {
        //Reflect
        direction = reflect(unitDirection, record->normal);
    }
    else 
    {
        //Refract
        direction = refract(unitDirection, record->normal, refractionIndex);
    }

    rayScattered->origin = record->p;
    rayScattered->direction = direction;
    rayScattered->invDirection = (Vec3){1/rayScattered->direction.x, 1/rayScattered->direction.y, 1/rayScattered->direction.z};
    return true;
}

Dielectric* createDielectric(double refractionIndex)
{
    Dielectric* mat = malloc(sizeof(Dielectric));
    mat->material = createDefaultMaterial();
    mat->material.scatter = scatterDielectric;
    mat->material.destroy = destoryDielectric;
    mat->refractionIndex = refractionIndex;
    return mat;
}

void destoryDielectric(void* dielectric)
{
    free(dielectric);
}
