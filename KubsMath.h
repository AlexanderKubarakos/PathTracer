#pragma once
#include <float.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

static inline double radianToDegree(double radian)
{
    return radian * 180.0 / M_PI;
}

static inline double degreeToRadian(double degree)
{
    return degree * M_PI / 180.0;
}
static unsigned int popper = 0;
// random double [0, 1)
static inline double randomDouble()
{
    return rand_r(&popper) / (RAND_MAX + 1.0);
}

static inline double randomDoubleRange(double min, double max)
{
    return randomDouble() * (max-min) + min;
}

static inline double clamp(double value, double min, double max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}
