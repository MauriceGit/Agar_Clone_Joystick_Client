#ifndef __MTVECTOR_H__
#define __MTVECTOR_H__
/**
 * Interface for basic vector calculations.
 * All operations are prefixed with 'mt' to avoid name clashes and get a
 * attempt for a unique prefix.
 *
 * @author Maurice Tollmien
 */

#include <stdarg.h>

typedef struct {
    float x;
    float y;
    float z;
} Vec3D;

#define MT_PI 3.14159265

Vec3D mtToVector3D(float x, float y, float z);
float mtVectorLength3D(Vec3D vector);
Vec3D mtNormVector3D(Vec3D vector);
Vec3D mtCrossProduct3D(Vec3D a, Vec3D b);
Vec3D mtMultiplyVectorScalar (Vec3D a, double s);
Vec3D mtSubtractVectorVector (Vec3D a, Vec3D b);
Vec3D mtDivideVectorScalar (Vec3D a, double s);
Vec3D mtAddVectorVector (Vec3D a, Vec3D b);
void  mtPrintVector (Vec3D a);
double mtAngleVectorVector (Vec3D a, Vec3D b);
double mtRadToDeg (double rad);
double mtDegToRad (double deg);
double mtScalarProduct (Vec3D a, Vec3D b);

#endif
