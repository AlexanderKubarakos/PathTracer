#pragma once

#include "Scene.h"
#include "PPM.h"
// Camera renders a scene with certain view parameters (sample counts, depth, ...)
typedef struct
{
    int width;
    int height;
    double aspectRatio;
    double vfov;
    int sampleCount;
    int rayDepth;
    double defocusAngle;
    double focusDist;
    Vec3 defocusDiskU;
    Vec3 defocusDiskV;
    Vec3 lookFrom;
    Vec3 lookAt;
    Vec3 up;
    Vec3 center;
    Vec3 pixel00;
    Vec3 pixelDeltaU;
    Vec3 pixelDeltaV;
} Camera;

// Data passed to each thread, describes what to render and where
typedef struct {
    Image* image; //image to draw too
    Camera* camera; //camera to use in render
    Scene* scene; //scene to hit against
    bool done; // TODO: maybe make this a condition var so I can spin lock check it for true
    int nextTile; //shared state for all threads, tells thread what tile to do next
    int threadCount;
    pthread_mutex_t mutex; // mutex for nextTile mutal exclussion
} RenderTarget;

Camera* createCamera(int width, double aspectRatio, int sampleCount, int rayDepth, double vfov, double defocusAngle, double focusDistance);
void lookAt(Camera* camera, const Vec3 lookFrom, const Vec3 lookAt, const Vec3 up);
RenderTarget* renderScene(Camera* camera, Scene* scene, int threadCount);
void destroyCamera(Camera* camera);
