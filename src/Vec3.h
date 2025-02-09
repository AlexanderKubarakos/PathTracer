#pragma once
#include "KubsMath.h"
#include <math.h>
#include <stdbool.h>

typedef struct 
{
    double x;
    double y;
    double z;
} Vec3;

/* TODO: Rewrite this stuff
addVec3F
addVec3
subVec3F
subVec3
mulVec3F
mulVec3
*/

static inline Vec3 addVec3(const Vec3 a, const Vec3 b) { return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};}
static inline Vec3 subVec3(const Vec3 a, const Vec3 b) { return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};}
static inline Vec3 mulVec3(const Vec3 a, double d) { return (Vec3){a.x*d, a.y*d, a.z*d};}
static inline Vec3 mulVec3Vec3(const Vec3 a, const Vec3 b) {return (Vec3){a.x * b.x, a.y * b.y, a.z * b.z};}
static inline Vec3 divVec3(const Vec3 a, double d) { return (Vec3){a.x/d, a.y/d, a.z/d};}
static inline Vec3 negVec3(const Vec3 a) { return (Vec3){a.x * -1.0, a.y * -1.0, a.z * -1.0};}

static inline double lengthSquared(const Vec3 a) {return a.x*a.x + a.y*a.y + a.z*a.z;}
static inline double dot(const Vec3 a, const Vec3 b) {return a.x*b.x +a.y*b.y + a.z*b.z;}
static inline double length(const Vec3 a) {return sqrt(lengthSquared(a));}
static inline Vec3 unitVector(const Vec3 a) {return divVec3(a, length(a));};

static inline Vec3 cross(const Vec3 a, const Vec3 b) {
    return (Vec3){a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x};
}

static inline Vec3 randomVector(double min, double max) {return (Vec3){randomDoubleRange(min, max), randomDoubleRange(min, max), randomDoubleRange(min, max)};}
static inline Vec3 randomUnitVector()
{
    while (1)
    {
        Vec3 vec = randomVector(-1, 1);
        double lengthSq = lengthSquared(vec);
        if (1e-160 < lengthSq && lengthSq <= 1)
            return divVec3(vec, sqrt(lengthSq));
    }
}

static inline Vec3 randomInUnitDisc() {
    while (true) {
        Vec3 p = (Vec3){randomDoubleRange(-1,1), randomDoubleRange(-1,1), 0};
        if (lengthSquared(p) < 1)
            return p;
    }
}


static inline Vec3 randomOnHemisphere(const Vec3 normal)
{
    Vec3 vec = randomUnitVector();
    if (dot(vec, normal) > 0)
        return vec;
    else
        return negVec3(vec);
}

static inline Vec3 reflect(const Vec3 v, const Vec3 n) {
    return subVec3(v, mulVec3(mulVec3(n, dot(v,n)), 2));
}

static inline bool nearZero(const Vec3 a) 
{
    // Return true if the vector is close to zero in all dimensions.
    double s = 1e-8;
    return (fabs(a.x) < s) && (fabs(a.y) < s) && (fabs(a.z) < s);
}

static inline Vec3 refract(const Vec3 uv, const Vec3 n, double etaiOverEtat)
{
    double cosTheta = fmin(dot(negVec3(uv), n), 1.0);
    Vec3 rOutPerp = mulVec3(addVec3(uv, mulVec3(n, cosTheta)), etaiOverEtat);
    Vec3 rOutParallel = mulVec3(n, -sqrt(fabs(1.0 - lengthSquared(rOutPerp))));
    return addVec3(rOutPerp, rOutParallel);
}