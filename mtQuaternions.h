#ifndef __MTQUATERNIONS_H__
#define __MTQUATERNIONS_H__
/**
 * Interface for some operations on Quaternions.
 * All operations are prefixed with 'mt' to avoid name clashes and get an
 * attempt for a unique prefix.
 *
 * @author Maurice Tollmien
 */

#include "mtVector.h"

/** Quaternion */
typedef struct
{
    double s;
    MTVec3D v;
} MTQuaternion;

/* Low level operations on MTQuaternions */

MTQuaternion mtCreateQuaternion(MTVec3D axis, double angle);

MTQuaternion mtMultQuaternionQuaternion (const MTQuaternion* q1, const MTQuaternion* q2);

MTQuaternion mtMultQuaternionScalar (const MTQuaternion* q1, double s);

MTQuaternion mtAddQuaternionQuaternion (const MTQuaternion* q1, const MTQuaternion* q2);

MTQuaternion mtSubtractQuaternionQuaternion (const MTQuaternion* q1, const MTQuaternion* q2);

MTQuaternion mtConjugateQuaternion (const MTQuaternion* q1);

MTQuaternion mtInverseQuaternion (const MTQuaternion* q1);

void mtNormQuaternion (MTQuaternion* q1);

double mtLengthQuaternion (const MTQuaternion* q1);

int mtIsNormQuaternion (const MTQuaternion* q1);

/* Some higher level functions, using MTQuaternions */

MTVec3D mtRotatePointWithQuaternion(MTQuaternion q, MTVec3D point);
MTVec3D mtRotatePointAxis (MTVec3D axis, double angle, MTVec3D point);

#endif



