#ifndef __QUATERNIONS_H__
#define __QUATERNIONS_H__
/**
 * Interface for some operations on Quaternions.
 *
 * @author Maurice Tollmien
 */

#include "vector.h"

/** Quaternion */
typedef struct
{
    double s;
    Vec3D v;
} Quaternion;

/* Low level operations on Quaternions */

Quaternion createQuaternion(Vec3D axis, double angle);

void multQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion * res);

void multQuaternionScalar (const Quaternion* q1, double s, Quaternion* res);

void addQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion* res);

void subtractQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion* res);

void conjugateQuaternion (Quaternion* q1);

void inverseQuaternion (const Quaternion* q1, Quaternion* res);

void normQuaternion (Quaternion* q1);

double lengthQuaternion (const Quaternion* q1);

int isNormQuaternion (const Quaternion* q1);

/* Some higher level functions, using Quaternions */

void rotatePointWithQuaternion(Quaternion q, Vec3D * point);
void rotatePointAxis (Vec3D axis, double angle, Vec3D * point);

#endif



