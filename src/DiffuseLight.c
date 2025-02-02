#include "DiffuseLight.h"

Color diffuseEmission(void* diffuseLight)
{
    return ((DiffuseLight*)diffuseLight)->color;
}

DiffuseLight* createDiffuseLight(Color color)
{
    DiffuseLight* light = malloc(sizeof(DiffuseLight));
    light->color = color;
    light->material = createDefaultMaterial();
    light->material.emission = diffuseEmission;
    return light;
}
