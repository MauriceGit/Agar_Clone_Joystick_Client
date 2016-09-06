#ifndef __VECTOR_H__
#define __VECTOR_H__
#define __DEBUG_GL_H__
/**
 * @file
 * Vector operations.
 *
 * @author Maurice Tollmien
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdarg.h>


/* eig. Funktionen */

typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Vec3D;

Vec3D toVector3D(GLfloat x, GLfloat y, GLfloat z);
float vectorLength3D(Vec3D vector);
Vec3D normVector3D(Vec3D vector);
Vec3D crossProduct3D(Vec3D a, Vec3D b);
Vec3D multiplyVectorScalar (Vec3D a, double s);
Vec3D subtractVectorVector (Vec3D a, Vec3D b);
Vec3D divideVectorScalar (Vec3D a, double s);
Vec3D addVectorVector (Vec3D a, Vec3D b);
void printVector (Vec3D a);
double angleVectorVector (Vec3D a, Vec3D b);
double radToDeg (double rad);
double degToRad (double deg);
double scalarProduct (Vec3D a, Vec3D b);

#endif
