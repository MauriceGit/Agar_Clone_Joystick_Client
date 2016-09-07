/**
 * Implementation of most relevant functions on Quaternions.
 *
 * All operations are prefixed with 'mt' to avoid name clashes and get a
 * attempt for a unique prefix.
 *
 * @author Maurice Tollmien
 */

/* ---- System Header ---- */
#include <stdlib.h>
#include <math.h>
#include "types.h"

/* ---- My Header ---- */
#include "mtQuaternions.h"
#include "mtVector.h"

#define EPS     0.0001

/*
 * Low level operations on Quaternions
 */
Quaternion mtCreateQuaternion(Vec3D axis, double angle) {
    Quaternion q;
    q.s = cos (angle/2.0);
    q.v = mtMultiplyVectorScalar(axis, sin(angle/2.0));
    return q;
}

/**
 * Multiply to Quaternions with each other.
 * Careful! Not commutative!!!
 * Calculates: q1 * q2
 */
Quaternion mtMultQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2)
{
    Quaternion res;
    res.s = q1->s*q2->s - mtScalarProduct(q1->v, q2->v);
    Vec3D vres = mtCrossProduct3D(q1->v, q2->v);

    Vec3D tmp = mtMultiplyVectorScalar (q2->v, q1->s);
    vres = mtAddVectorVector(vres, tmp);
    tmp = mtMultiplyVectorScalar(q1->v, q2->s);
    res.v = mtAddVectorVector(vres, tmp);
    return res;
}

/**
 * Multiplies a Quaternion and a scalar.
 * Therefore the scalar will be converted to a Quaternion.
 * After that the two Quaternions will be muliplied.
 */
Quaternion mtMultQuaternionScalar (const Quaternion* q1, double s)
{
    Quaternion q2;

    q2.s = s;
    q2.v = {.x=0.0, .y=0.0, .z=0.0};

    return mtMultQuaternionQuaternion (q1, &q2);
}

/**
 * Calculates: q1 + q2.
 */
Quaternion mtAddQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2)
{
    Quaternion res;
    res.s = q1->s + q2->s;
    res.v = mtAddVectorVector(q1->v, q2->v);
    return res;
}

/**
 * Calculates q1 - q2.
 */
Quaternion mtSubtractQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2)
{
    Quaternion res;
    res.s = q1->s - q2->s;
    res.v = mtSubtractVectorVector(q1->v, q2->v);
    return res;
}

/**
 * Complex conjugate the Quaternion.
 */
void mtConjugateQuaternion (Quaternion* q1)
{
    q1->v = mtMultiplyVectorScalar(q1->v, -1.0);
}

/**
 * Invert the Quaternion.
 */
Quaternion mtInverseQuaternion (const Quaternion* q1)
{
    Quaternion res;
    double qlen = pow (mtLengthQuaternion (q1), 2);

    Quaternion tmp = *q1;

    mtConjugateQuaternion(&tmp);

    return mtMultQuaternionScalar (&tmp, 1.0 / qlen);
}

/**
 * Normalize the Quaternion to a length of 1.
 */
void mtNormQuaternion (Quaternion* q1)
{
    double qlen = mtLengthQuaternion (q1);

    q1->s /= qlen;
    q1->v = mtMultiplyVectorScalar(q1->v, 1.0 / qlen);
}

/**
 * Calculates the length of the Quaternion.
 */
double mtLengthQuaternion (const Quaternion* q1)
{
    return sqrt (q1->s*q1->s + q1->v.x*q1->v.x + q1->v.y*q1->v.y + q1->v.z*q1->v.z);
}

/**
 * Check if the Quaternion is normalized.
 */
int mtIsNormQuaternion (const Quaternion* q1)
{
    double res = q1->s*q1->s + q1->v.x*q1->v.x + q1->v.y*q1->v.y + q1->v.z*q1->v.z;
    return (res + EPS >= 1.0) && (res - EPS <= 1.0);
}

/* Some higher level functions, using Quaternions */

Vec3D mtRotatePointWithQuaternion(Quaternion q, Vec3D point)
{
    mtNormQuaternion(&q);

    // Create Quaternion of the point to rotate
    Quaternion p;
    p.s    = 0.0;
    p.v = point;

    // The actual calculations.
    //  ---  q p q*  ---
    Quaternion inverseQ = mtInverseQuaternion(&q);
    Quaternion res = mtMultQuaternionQuaternion (&q, &p);
    res = mtMultQuaternionQuaternion (&res, &inverseQ);

    // Write new rotated coordinates back to the point
    return res.v;
}

/**
 * Rotates a given point around a given axis by a given angle.
 * The rotations uses Quaternions internally and writes the rotated (modified)
 * coordinates back to the point.
 */
Vec3D mtRotatePointAxis (Vec3D axis, double angle, Vec3D point)
{
    // create Quaternion from axis and angle
    Quaternion q;
    q.s = cos (angle/2.0);
    q.v = mtMultiplyVectorScalar(axis, sin(angle/2.0));

    return mtRotatePointWithQuaternion(q, point);

}
