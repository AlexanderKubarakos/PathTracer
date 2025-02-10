#include "Model.h"

#include <stdio.h>
#include <string.h>

bool modelHit(void* this, const Ray ray, double rayMin, double rayMax, HitRecord* record)
{
    HitRecord tempRecord;
    bool hitAnything = false;
    double closestSoFar = rayMax;

    Model* model = (Model*)this;
    for (int i = 0; i < model->triangleCount; i++)
    {
        if(!triangleRay(ray, model->triangles[i], rayMin, closestSoFar, &tempRecord))
            continue;
        hitAnything = true;
        closestSoFar = tempRecord.t;
        *record = tempRecord;
        record->material = model->h.material;
    }

    return hitAnything;
}

Model* loadOBJModel(const char* fileLocation)
{
    FILE* file;
    file = fopen(fileLocation, "r");
    if (!file)
    {
        printf("Failed to open model file at: %s\n", fileLocation);
        return NULL;
    }

    Model* model = malloc(sizeof(Model));

    Vec3* vertexList = malloc(sizeof(Vec3) * 10000);
    int vertex = 0;

    model->triangles = malloc(sizeof(Triangle) * 10000);
    model->triangleCount = 0;

    char data[128];
    char* token;
    while (fgets(data, 128, file) != NULL)
    {
        if (data[0] != 'v' && data[0] != 'f')
            continue; // Not usefull line of data
        if (data[0] == 'v' && data[1] == 'n')
            continue; // To be used...

        if (data[0] == 'v' && data[1] == ' ')
        {
            token = strtok(data+2, " ");
            vertexList[vertex].x = strtof(token, &token);
            token = strtok(NULL, " ");
            vertexList[vertex].y = strtof(token, &token);
            vertexList[vertex].z = strtof(token+1, &token);
            vertex++;
        }
        else if (data[0] == 'f')
        {
            Triangle t;
            token = strtok(data+2, " ");
            t.a = vertexList[atoi(token)-1];
            token = strtok(NULL, " ");
            t.b = vertexList[atoi(token)-1];
            token = strtok(NULL, " ");
            t.c = vertexList[atoi(token)-1];
            model->triangles[model->triangleCount++] = t;
        }
    }

    //TEMP
    model->h.hitfunc = &modelHit;
    model->h.material = (Material*)createLambertian((Color){0.2,0.2,0.2});
    fclose(file);
    return model;
}

    static Vec3 v[] = {
        (Vec3){-1, -1,  1}, //0
        (Vec3){1, -1,  1}, //1
        (Vec3){-1,  1,  1 },//2
         (Vec3){1,  1,  1 },//3
        (Vec3){-1, -1, -1 },//4
        (Vec3){ 1, -1, -1 },//5
        (Vec3){-1,  1, -1}, //6
        (Vec3){ 1,  1, -1}  //7
    };

void defaultCube(Model* model)
{
    model->triangles = malloc(sizeof(Triangle) * 10000);
    model->triangleCount = 0;

    model->triangles[model->triangleCount++] = (Triangle){v[2], v[6], v[7]}; // top
    model->triangles[model->triangleCount++] = (Triangle){v[2], v[7], v[3]};
    model->triangles[model->triangleCount++] = (Triangle){v[0], v[5], v[4]}; // bot
    model->triangles[model->triangleCount++] = (Triangle){v[0], v[1], v[5]};
    model->triangles[model->triangleCount++] = (Triangle){v[0], v[2], v[6]}; // back
    model->triangles[model->triangleCount++] = (Triangle){v[0], v[4], v[6]};
    model->triangles[model->triangleCount++] = (Triangle){v[1], v[3], v[7]}; // front // front and bottom are over writting
    model->triangles[model->triangleCount++] = (Triangle){v[1], v[5], v[7]};
    model->triangles[model->triangleCount++] = (Triangle){v[0], v[2], v[3]};
    model->triangles[model->triangleCount++] = (Triangle){v[0], v[1], v[3]};
    model->triangles[model->triangleCount++] = (Triangle){v[4], v[6], v[7]}; // back right
    model->triangles[model->triangleCount++] = (Triangle){v[4], v[5], v[7]};

    model->h.hitfunc = &modelHit;
    model->h.material = (Material*)createLambertian((Color){0.2,0.2,0.2});
}

void freeModel(Model* model)
{
    //TODO
}