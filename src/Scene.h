#pragma once
#include "Hittable.h"
#include "BVH.h"
#include "PPM.h"
#include "Model.h"
// A scene is a list of hitable objects
typedef struct
{
    HittableList list;
    Model* models;
    int modelCount;
    Color background;
} Scene;

Scene* createScene(int length, Color background);
void destroyScene(Scene* scene);
void addHittable(Scene* scene, Hittable* hittable);
void sceneAddModel(Scene* scene, Model* model);
void buildSceneBVH(Scene* scene);
bool hitScene(Scene* scene, const Ray ray, double rayMin, double rayMax, HitRecord* record);