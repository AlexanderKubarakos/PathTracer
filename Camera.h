#pragma once

#include "Scene.h"

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

Camera* createCamera(int width, double aspectRatio, int sampleCount, int rayDepth, double vfov, double defocusAngle, double focusDistance);
void lookAt(Camera* camera, const Vec3 lookFrom, const Vec3 lookAt, const Vec3 up);
void renderScene(Camera* camera, Scene* scene, int threadCount);
void destroyCamera(Camera* camera);
