#ifndef __MTQUATERNIONS_H__
#define __MTQUATERNIONS_H__
/**
 * Interface for some operations on Quaternions.
 * All operations are prefixed with 'mt' to avoid name clashes and get a
 * attempt for a unique prefix.
 *
 * @author Maurice Tollmien
 */

#include "mtVector.h"

/** Quaternion */
typedef struct
{
    double s;
    Vec3D v;
} Quaternion;

/* Low level operations on Quaternions */

Quaternion mtCreateQuaternion(Vec3D axis, double angle);

Quaternion mtMultQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2);

Quaternion mtMultQuaternionScalar (const Quaternion* q1, double s);

Quaternion mtAddQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2);

Quaternion mtSubtractQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2);

void mtConjugateQuaternion (Quaternion* q1);

Quaternion mtInverseQuaternion (const Quaternion* q1);

void mtNormQuaternion (Quaternion* q1);

double mtLengthQuaternion (const Quaternion* q1);

int mtIsNormQuaternion (const Quaternion* q1);

/* Some higher level functions, using Quaternions */

Vec3D mtRotatePointWithQuaternion(Quaternion q, Vec3D point);
Vec3D mtRotatePointAxis (Vec3D axis, double angle, Vec3D point);

#endif



