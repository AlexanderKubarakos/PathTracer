#include "Material.h"

bool defaultScatter(void* this, const Ray rayIn, HitRecord* record, Color* attenuation, Ray* rayScattered)
{
    (void) this; // GCC shutup! :)
    (void) rayIn;
    (void) record;
    (void) attenuation;
    (void) rayScattered;
    return false;
}

Color defaultEmission(void* this)
{
    (void)this; // gcc shut up 2.0 electric boogaloo
    return (Color){0,0,0};
}


Material createDefaultMaterial()
{
    // no default destruction function, to make sure you handle memory well :)
    return (Material){.scatter=defaultScatter, .emission=defaultEmission, .destroy=NULL};
}

void destroyMaterial(Material* this)
{
    this->destroy(this);
}
