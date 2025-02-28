#include "Model.h"
#include <stdio.h>
#include <string.h>

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
    model->material = (Material*)createLambertian((Color){0.2,0.2,0.2});
    
    Vec3* vertexList = malloc(sizeof(Vec3) * 1000000);
    int vertex = 0;

    TriangleList triangleList = triangleListCreate(1);

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
            // TODO: add re alloc
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
            triangleListAdd(&triangleList, t);
        }
    }

    model->bvh = createBVH(&triangleList);

    free(vertexList);
    triangleListDestroy(&triangleList);
    fclose(file);
    return model;
}

void deleteModel(Model* model)
{
    
}