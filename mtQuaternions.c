/**
 * Implementation of most relevant functions on MTQuaternions.
 *
 * All operations are prefixed with 'mt' to avoid name clashes and get an
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
 * Low level operations on MTQuaternions
 */
MTQuaternion mtCreateQuaternion(MTVec3D axis, double angle) {
    MTQuaternion q;
    q.s = cos (angle/2.0);
    q.v = mtMultiplyVectorScalar(axis, sin(angle/2.0));
    return q;
}

/**
 * Multiply to MTQuaternions with each other.
 * Careful! Not commutative!!!
 * Calculates: q1 * q2
 */
MTQuaternion mtMultQuaternionQuaternion (const MTQuaternion* q1, const MTQuaternion* q2)
{
    MTQuaternion res;
    res.s = q1->s*q2->s - mtScalarProduct(q1->v, q2->v);
    MTVec3D vres = mtCrossProduct3D(q1->v, q2->v);

    MTVec3D tmp = mtMultiplyVectorScalar (q2->v, q1->s);
    vres = mtAddVectorVector(vres, tmp);
    tmp = mtMultiplyVectorScalar(q1->v, q2->s);
    res.v = mtAddVectorVector(vres, tmp);
    return res;
}

/**
 * Multiplies a MTQuaternion and a scalar.
 * Therefore the scalar will be converted to a MTQuaternion.
 * After that the two MTQuaternions will be muliplied.
 */
MTQuaternion mtMultQuaternionScalar (const MTQuaternion* q1, double s)
{
    MTQuaternion q2;

    q2.s = s;
    q2.v = mtToVector3D(0,0,0);

    return mtMultQuaternionQuaternion (q1, &q2);
}

/**
 * Calculates: q1 + q2.
 */
MTQuaternion mtAddQuaternionQuaternion (const MTQuaternion* q1, const MTQuaternion* q2)
{
    MTQuaternion res;
    res.s = q1->s + q2->s;
    res.v = mtAddVectorVector(q1->v, q2->v);
    return res;
}

/**
 * Calculates q1 - q2.
 */
MTQuaternion mtSubtractQuaternionQuaternion (const MTQuaternion* q1, const MTQuaternion* q2)
{
    MTQuaternion res;
    res.s = q1->s - q2->s;
    res.v = mtSubtractVectorVector(q1->v, q2->v);
    return res;
}

/**
 * Complex conjugate the MTQuaternion.
 */
MTQuaternion mtConjugateQuaternion (const MTQuaternion* q1)
{
    MTQuaternion res;
    res.s = q1->s;
    res.v = mtMultiplyVectorScalar(q1->v, -1.0);
    return res;
}

/**
 * Invert the MTQuaternion.
 */
MTQuaternion mtInverseQuaternion (const MTQuaternion* q1)
{
    MTQuaternion res;
    double qlen = pow (mtLengthQuaternion (q1), 2);

    MTQuaternion tmp = mtConjugateQuaternion(q1);

    return mtMultQuaternionScalar (&tmp, 1.0 / qlen);
}

/**
 * Normalize the MTQuaternion to a length of 1.
 */
void mtNormQuaternion (MTQuaternion* q1)
{
    double qlen = mtLengthQuaternion (q1);

    q1->s /= qlen;
    q1->v = mtMultiplyVectorScalar(q1->v, 1.0 / qlen);
}

/**
 * Calculates the length of the MTQuaternion.
 */
double mtLengthQuaternion (const MTQuaternion* q1)
{
    return sqrt (q1->s*q1->s + q1->v.x*q1->v.x + q1->v.y*q1->v.y + q1->v.z*q1->v.z);
}

/**
 * Check if the MTQuaternion is normalized.
 */
int mtIsNormQuaternion (const MTQuaternion* q1)
{
    double res = q1->s*q1->s + q1->v.x*q1->v.x + q1->v.y*q1->v.y + q1->v.z*q1->v.z;
    return (res + EPS >= 1.0) && (res - EPS <= 1.0);
}

/* Some higher level functions, using MTQuaternions */

MTVec3D mtRotatePointWithQuaternion(MTQuaternion q, MTVec3D point)
{
    mtNormQuaternion(&q);

    // Create MTQuaternion of the point to rotate
    MTQuaternion p;
    p.s    = 0.0;
    p.v = point;

    // The actual calculations.
    //  ---  q p q*  ---
    MTQuaternion inverseQ = mtInverseQuaternion(&q);
    MTQuaternion res = mtMultQuaternionQuaternion (&q, &p);
    res = mtMultQuaternionQuaternion (&res, &inverseQ);

    // Write new rotated coordinates back to the point
    return res.v;
}

/**
 * Rotates a given point around a given axis by a given angle.
 * The rotations uses MTQuaternions internally and writes the rotated (modified)
 * coordinates back to the point.
 */
MTVec3D mtRotatePointAxis (MTVec3D axis, double angle, MTVec3D point)
{
    // create MTQuaternion from axis and angle
    MTQuaternion q;
    q.s = cos (angle/2.0);
    q.v = mtMultiplyVectorScalar(axis, sin(angle/2.0));

    return mtRotatePointWithQuaternion(q, point);

}
