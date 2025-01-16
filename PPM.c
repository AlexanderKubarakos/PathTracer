#include "PPM.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "KubsMath.h"

Image createImage(int w, int h)
{
    #if THREADING
    // TODO: init mutex
    #endif
    return (Image){w, h, malloc(w * h * 3 * sizeof(unsigned char))};
}

void deleteImage(Image image)
{
    #if THREADING
    //TODO: delete mutex
    #endif
    free(image.data);
}

double static inline linearToGamma(double linear)
{
    if (linear > 0)
        return sqrt(linear);
    return 0;
}

void setPixel(Image* image, int x, int y, Color* color)
{
    image->data[x*3 + y * image->width * 3] = (int)(256  * clamp(linearToGamma(color->x), 0, 0.999));
    image->data[x*3 + y * image->width * 3 + 1] = (int)(256  * clamp(linearToGamma(color->y), 0, 0.999));
    image->data[x*3 + y * image->width * 3 + 2] = (int)(256  * clamp(linearToGamma(color->z), 0, 0.999));
}

#if THREADING
void setPixelThreaded(Image* image, int x, int y, Color* color)
{

    image->data[x*3 + y * image->width * 3] = (int)(256  * clamp(linearToGamma(color->x), 0, 0.999));
    image->data[x*3 + y * image->width * 3 + 1] = (int)(256  * clamp(linearToGamma(color->y), 0, 0.999));
    image->data[x*3 + y * image->width * 3 + 2] = (int)(256  * clamp(linearToGamma(color->z), 0, 0.999));
}
#endif

void outputImage(Image* image, const char* file)
{
    FILE* imageFile;

    imageFile = fopen(file, "w");
    if (imageFile == NULL)
    {
        printf("Error writing to image\n");
        return;
    }

    fprintf(imageFile, "P3\n%i %i\n255\n", image->width, image->height);
    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            fprintf(imageFile, "%i %i %i\n", image->data[x*3 + y * image->width * 3], image->data[x*3 + y * image->width * 3 + 1], image->data[x*3 + y * image->width * 3 + 2]);
        }
    }

    fclose(imageFile);
}

Color randomColor()
{
    return (Color){randomDouble(), randomDouble(), randomDouble()};
}
