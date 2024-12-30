#include "Camera.h"
#include "KubsMath.h"
#include "PPM.h"
#include "Vec3.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Material.h"

void cameraSetup(Camera* c)
{
    c->height = (int)(c->width / c->aspectRatio);
    c->height = (c->height < 1) ? 1 : c->height;

    c->center = c->lookFrom;

    double theta = degreeToRadian(c->vfov);
    double h = tan(theta/2);
    double viewportHeight = 2.0 * h * c->focusDist;
    double viewportWidth = viewportHeight * ((double)c->width / c->height);

    Vec3 w = unitVector(subVec3(c->lookFrom, c->lookAt));
    Vec3 u = unitVector(cross(c->up, w));
    Vec3 v = cross(w, u);

    Vec3 viewportU = mulVec3(u, viewportWidth);
    Vec3 viewportV = mulVec3(v, -viewportHeight);

    c->pixelDeltaU = divVec3(viewportU, c->width);
    c->pixelDeltaV = divVec3(viewportV, c->height);

    Vec3 viewportUpperLeft = subVec3(c->center, mulVec3(w, c->focusDist));
    viewportUpperLeft = subVec3(viewportUpperLeft, divVec3(viewportU, 2));
    viewportUpperLeft = subVec3(viewportUpperLeft, divVec3(viewportV, 2));

    c->pixel00 = addVec3(viewportUpperLeft, 
    mulVec3(addVec3(c->pixelDeltaU, c->pixelDeltaV), 0.5));

    double defocusRadius = c->focusDist * tan(degreeToRadian(c->defocusAngle / 2));
    c->defocusDiskU = mulVec3(u, defocusRadius);
    c->defocusDiskV = mulVec3(v, defocusRadius);
}

Camera* createCamera(int width, double aspectRatio, int sampleCount, int rayDepth, double vfov, double defocusAngle, double focusDistance)
{
    Camera* c = malloc(sizeof(Camera));
    if (!c)
    {
        fprintf(stderr, "Failed to create camera\n");
        return c;
    }
    
    c->width = width;
    c->aspectRatio = aspectRatio;
    c->vfov = vfov;
    c->sampleCount = sampleCount;
    c->rayDepth = rayDepth;
    c->lookAt = (Vec3){0,0,-1};
    c->lookFrom = (Vec3){0,0,0};
    c->up = (Vec3){0,1,0};
    c->defocusAngle = defocusAngle;
    c->focusDist = focusDistance;

    cameraSetup(c);

    return c;
}

void lookAt(Camera* camera, const Vec3 lookFrom, const Vec3 lookAt, const Vec3 up)
{
    camera->lookFrom = lookFrom;
    camera->lookAt = lookAt;
    camera->up = up;
    cameraSetup(camera);
}

// Get the color of a ray in a scene
Color rayColor(Scene* scene, int depth, Ray ray)
{
    // Max bounces
    if (depth <= 0)
        return (Color){0,0,0};

    HitRecord record;
    // Does this ray hit anything?
    if (hitScene(scene, ray, 0.001, INT_MAX, &record))
    {
        Ray scattered;
        Color attenuation;
        if (record.material->scatter(record.material, ray, &record, &attenuation, &scattered))
        {
            return mulVec3Vec3(rayColor(scene, depth-1, scattered), attenuation);
        }
        return (Color){0,0,0};
    }

    // If we never reach the sky (or a light in the future) we will have black multiplied by attenuations of all the bounces
    // This creates black since black times any color is still black, so a ray will be return black if it never reaches the sky before max depth is reached
    // Removing this max depth still causes shadows to occur because so many bounces occur that the reapeated attudentions cause it to get close to black (multiplication with <1.0)
    // Didn't hit anything, return sky color
    Vec3 unitDirection = unitVector(ray.direction);
    double a = 0.5*(unitDirection.y + 1.0);
    return addVec3(mulVec3((Vec3){1,1,1}, 1.0-a), mulVec3((Vec3){0.5, 0.7, 1.0}, a));
}

static Vec3 defocusDiskSample(Camera* camera)
{
    Vec3 p = randomInUnitDisc();
    Vec3 ret = camera->center;
    ret = addVec3(ret, mulVec3(camera->defocusDiskU, p.x));
    ret = addVec3(ret, mulVec3(camera->defocusDiskV, p.y));
    return ret;
}

// Creates a radom ray in a pixel at [x, y]
Ray sampleRay(Camera* camera, int x, int y)
{
    Ray ray;
    ray.origin = (camera->defocusAngle <= 0) ? camera->center : defocusDiskSample(camera);

    Vec3 pixelCenter = {0,0,0};
    pixelCenter = addVec3(pixelCenter, camera->pixel00);
    pixelCenter = addVec3(pixelCenter, mulVec3(camera->pixelDeltaU, x + randomDoubleRange(-0.5, 0.5)));
    pixelCenter = addVec3(pixelCenter, mulVec3(camera->pixelDeltaV, y + randomDoubleRange(-0.5, 0.5)));
    ray.direction = subVec3(pixelCenter, ray.origin);

    return ray;
}

void renderScene(Camera* camera, Scene* scene)
{
    Image image = createImage(camera->width, camera->height);
    double colorRatio = 1.0 / camera->sampleCount;
    // Compute Image Data
    for (int y = 0; y < camera->height; y++)
    {
        if (y % (camera->height/10) == 0)
            printf("Scan Line Progress: %i %%\n", 10*(y / (camera->height / 10)));
        for (int x = 0; x < camera->width; x++)
        {
            Ray ray;
            Color pixelColor = {0,0,0};
            // Per sample
            for (int i = 0; i < camera->sampleCount; i++)
            {
                // Get random ray
                ray = sampleRay(camera, x, y);
                // Sum their colors
                pixelColor = addVec3(pixelColor, rayColor(scene, camera->rayDepth, ray));
            }

            pixelColor = mulVec3(pixelColor, colorRatio);
            
            setPixel(image, x, y, pixelColor);
        } 
    }

    fprintf(stderr, "Outputing image...\n");
    outputImage(image, "output2.ppm");
    deleteImage(image);
}

void destroyCamera(Camera* camera)
{
    free(camera);
}