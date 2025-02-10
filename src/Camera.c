#include "Camera.h"
#include "KubsMath.h"
#include "PPM.h"
#include "Vec3.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Material.h"
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#define TILE_SIZE 64

// Data passed to each thread, describes what to render and where
typedef struct {
    RenderResult* result;
    Camera* camera; //camera to use in render
    Scene* scene; //scene to hit against
    int nextTile; //shared state for all threads, tells thread what tile to do next
    _Atomic(int) threadCount;
} RenderTarget;

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
        printf("Failed to create camera\n");
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

void destroyCamera(Camera* camera)
{
    free(camera);
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
    if (!hitScene(scene, ray, 0.001, INT_MAX, &record))
        return scene->background;

     Ray scattered;
     Color attenuation;
     Color emissionColor = record.material->emission(record.material);

     if (!record.material->scatter(record.material, ray, &record, &attenuation, &scattered))
         return emissionColor;

     Color scatterColor = mulVec3Vec3(rayColor(scene, depth-1, scattered), attenuation);
     return addVec3(emissionColor, scatterColor);
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
    ray.invDirection = (Vec3){1/ray.direction.x, 1/ray.direction.y, 1/ray.direction.z};

    return ray;
}

void* renderTiles(void* data)
{
    RenderTarget* target = (RenderTarget*)data;

    Camera* camera = target->camera;
    Image* image = target->result->image;
    Scene* scene = target->scene;
    double colorRatio = 1.0 / camera->sampleCount;

    int xTileAmmount = camera->width / TILE_SIZE + (camera->width % TILE_SIZE == 0 ? 0 : 1);
    int yTileAmmount = camera->height / TILE_SIZE + (camera->height % TILE_SIZE == 0 ? 0 : 1);
    while(true)
    {
        // pull new tile
        int tile = target->nextTile++;

        // if this tile is out of bounds, we have finished our work
        if (tile >= xTileAmmount * yTileAmmount || target->result->stop)
        {
            return NULL;
        }

        // Get the top let corner of the tile
        int tileY = tile / xTileAmmount;
        int tileX = tile % xTileAmmount;
        for (int y = tileY * TILE_SIZE; y < tileY * TILE_SIZE + TILE_SIZE && y < image->height; y++)
        {
            for (int x = tileX * TILE_SIZE; x < tileX * TILE_SIZE + TILE_SIZE && x < image->width; x++)
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
                setPixel(image, x, y, &pixelColor); // set pixel is thread-safe
            }
        }

        // print progress
        if (tile % 4 == 0 && tile != 0)
        {
            printf("Progress %.2f%%\n", (float)tile/xTileAmmount/yTileAmmount * 100.0f);
        }
    }
    return NULL;
}

void* renderSceneThreaded(void* arg)
{
    RenderTarget* target = (RenderTarget*)arg;
    int threadCount = target->threadCount;
    pthread_t* threads = malloc(sizeof(*threads) * threadCount);

    printf("Starting Render\n");
    time_t start = time(NULL);
    for (int i = 0; i < threadCount; i++)
    {
        pthread_create(&threads[i], NULL, renderTiles, target);
    }

    for (int i = 0; i < threadCount; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //BUG: free target correctly
    if (target->result->stop)
    {
        printf("Stoping render mid-way...\n");
    }
    else
    {
        printf("Done Render! Time spent: %lis\n", time(NULL) - start);
        printf("Outputing image...\n");
        outputImage(target->result->image, "output.ppm");
    }
    target->result->done = true;
    free(target);
    return NULL;
}

RenderResult* renderScene(Camera* camera, Scene* scene, int threadCount)
{
    buildSceneBVH(scene);
    RenderResult* result = malloc(sizeof(*result));
    result->image = malloc(sizeof(Image));
    *result->image = createImage(camera->width, camera->height);
    result->done = false;

    RenderTarget* target = malloc(sizeof(*target));
    target->result = result;
    target->camera = camera;
    target->scene = scene;
    target->nextTile = 0;
    target->threadCount = threadCount;

    pthread_t thread;
    pthread_create(&thread, NULL, renderSceneThreaded, target);
    return result;
}
