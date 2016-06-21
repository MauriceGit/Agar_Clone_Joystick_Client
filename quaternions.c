/**
 * Implementation of most relevant functions on Quaternions.
 *
 * @author Maurice Tollmien
 */

/* ---- System Header ---- */
#include <stdlib.h>
#include <math.h>
#include "types.h"

/* ---- My Header ---- */
#include "quaternions.h"
#include "vector.h"

#define EPS     0.0001

/*
 * Low level operations on Quaternions
 */
Quaternion createQuaternion(Vec3D axis, double angle) {
    Quaternion q;
    q.s = cos (angle/2.0);
    q.v = multiplyVectorScalar(axis, sin(angle/2.0));
    return q;
}

/**
 * Multiply to Quaternions with each other.
 * Careful! Not commutative!!!
 * Calculates: q1 * q2
 */
void multQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion * res)
{
    res->s = q1->s*q2->s - scalarProduct(q1->v, q2->v);
    Vec3D vres = crossProduct3D(q1->v, q2->v);

    Vec3D tmp = multiplyVectorScalar (q2->v, q1->s);
    vres = addVectorVector(vres, tmp);
    tmp = multiplyVectorScalar(q1->v, q2->s);
    res->v = addVectorVector(vres, tmp);

}

/**
 * Multiplies a Quaternion and a scalar.
 * Therefore the scalar will be converted to a Quaternion.
 * After that the two Quaternions will be muliplied.
 */
void multQuaternionScalar (const Quaternion* q1, double s, Quaternion* res)
{
    Quaternion q2;

    q2.s = s;
    q2.v = {.x=0.0, .y=0.0, .z=0.0};

    multQuaternionQuaternion (q1, &q2, res);
}

/**
 * Calculates: q1 + q2.
 */
void addQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion* res)
{
    res->s = q1->s + q2->s;
    res->v = addVectorVector(q1->v, q2->v);
}

/**
 * Calculates q1 - q2.
 */
void subtractQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion* res)
{
    res->s = q1->s - q2->s;
    res->v = subtractVectorVector(q1->v, q2->v);
}

/**
 * Complex conjugate the Quaternion.
 */
void conjugateQuaternion (Quaternion* q1)
{
    q1->v = multiplyVectorScalar(q1->v, -1.0);
}

/**
 * Invert the Quaternion.
 */
void inverseQuaternion (const Quaternion* q1, Quaternion* res)
{
    double qlen = pow (lengthQuaternion (q1), 2);

    Quaternion tmp = *q1;

    conjugateQuaternion(&tmp);

    multQuaternionScalar (&tmp, 1.0 / qlen, res);
}

/**
 * Normalize the Quaternion to a length of 1.
 */
void normQuaternion (Quaternion* q1)
{
    double qlen = lengthQuaternion (q1);

    q1->s /= qlen;
    q1->v = multiplyVectorScalar(q1->v, 1.0 / qlen);
}

/**
 * Calculates the length of the Quaternion.
 */
double lengthQuaternion (const Quaternion* q1)
{
    return sqrt (q1->s*q1->s + q1->v.x*q1->v.x + q1->v.y*q1->v.y + q1->v.z*q1->v.z);
}

/**
 * Check if the Quaternion is normalized.
 */
int isNormQuaternion (const Quaternion* q1)
{
    double res = q1->s*q1->s + q1->v.x*q1->v.x + q1->v.y*q1->v.y + q1->v.z*q1->v.z;
    return (res + EPS >= 1.0) && (res - EPS <= 1.0);
}

/* Some higher level functions, using Quaternions */

void rotatePointWithQuaternion(Quaternion q, Vec3D * point)
{
    normQuaternion(&q);

    Quaternion p;
    Quaternion res;
    Quaternion inverseQ;

    // Create Quaternion of the point to rotate
    p.s    = 0.0;
    p.v = *point;

    // The actual calculations.
    //  ---  q p q*  ---
    inverseQuaternion(&q, &inverseQ);
    multQuaternionQuaternion (&q, &p, &res);
    multQuaternionQuaternion (&res, &inverseQ, &res);

    // Write new rotated coordinates back to the point
    *point = res.v;
}

/**
 * Rotates a given point around a given axis by a given angle.
 * The rotations uses Quaternions internally and writes the rotated (modified)
 * coordinates back to the point.
 */
void rotatePointAxis (Vec3D axis, double angle, Vec3D * point)
{
    // create Quaternion from axis and angle
    Quaternion q;
    q.s = cos (angle/2.0);
    q.v = multiplyVectorScalar(axis, sin(angle/2.0));

    rotatePointWithQuaternion(q, point);

}
