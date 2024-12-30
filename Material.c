#include "Material.h"

void destroyMaterial(Material* this)
{
    this->destroy(this);
}