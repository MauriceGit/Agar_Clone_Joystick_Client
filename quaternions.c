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
    //Vec3D tmp = axis;
    //tmp[0] = axis[0];
    //tmp[1] = axis[1];
    //tmp[2] = axis[2];
    q.s = cos (angle/2.0);
    //multiplyVectorScalar_intern (axis, sin(angle/2.0), &tmp);
    q.v = multiplyVectorScalar(axis, sin(angle/2.0));
    //q.v[0] = tmp[0];
    //q.v[1] = tmp[1];
    //q.v[2] = tmp[2];
    return q;
}

/**
 * Multiply to Quaternions with each other.
 * Careful! Not commutative!!!
 * Calculates: q1 * q2
 */
void multQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion * res)
{
    //Vec3D vres;
    //Vec3D tmp;

    //res->s = q1->s*q2->s - multiplyVectorVector_intern(q1->v, q2->v);
    res->s = q1->s*q2->s - scalarProduct(q1->v, q2->v);

    //crossProduct3D_intern (&vres, q1->v, q2->v);
    Vec3D vres = crossProduct3D(q1->v, q2->v);

    Vec3D tmp = multiplyVectorScalar (q2->v, q1->s);
    //addVectorVector_intern (vres, tmp, &vres);
    vres = addVectorVector(vres, tmp);
    //multiplyVectorScalar_intern (q1->v, q2->s, &tmp);
    tmp = multiplyVectorScalar(q1->v, q2->s);
    //addVectorVector_intern (vres, tmp, &vres);
    res->v = addVectorVector(vres, tmp);

    //res->v[0] = vres[0];
    //res->v[1] = vres[1];
    //res->v[2] = vres[2];
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
    //q2.v[0] = 0.0;
    //q2.v[1] = 0.0;
    //q2.v[2] = 0.0;

    multQuaternionQuaternion (q1, &q2, res);
}

/**
 * Calculates: q1 + q2.
 */
void addQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion* res)
{
    res->s = q1->s + q2->s;
    //res->v[0] = q1->v[0] + q2->v[0];
    //res->v[1] = q1->v[1] + q2->v[1];
    //res->v[2] = q1->v[2] + q2->v[2];
    res->v = addVectorVector(q1->v, q2->v);
}

/**
 * Calculates q1 - q2.
 */
void subtractQuaternionQuaternion (const Quaternion* q1, const Quaternion* q2, Quaternion* res)
{
    res->s = q1->s - q2->s;
    //res->v[0] = q1->v[0] - q2->v[0];
    //res->v[1] = q1->v[1] - q2->v[1];
    //res->v[2] = q1->v[2] - q2->v[2];
    res->v = subtractVectorVector(q1->v, q2->v);
}

/**
 * Complex conjugate the Quaternion.
 */
void conjugateQuaternion (Quaternion* q1)
{
    //q1->v[0] *= -1.0;
    //q1->v[1] *= -1.0;
    //q1->v[2] *= -1.0;
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
    //q1->v[0] /= qlen;
    //q1->v[1] /= qlen;
    //q1->v[2] /= qlen;
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
    //p.v[0] = (*point)[0];
    //p.v[1] = (*point)[1];
    //p.v[2] = (*point)[2];
    p.v = *point;

    // The actual calculations.
    //  ---  q p q*  ---
    inverseQuaternion(&q, &inverseQ);
    multQuaternionQuaternion (&q, &p, &res);
    multQuaternionQuaternion (&res, &inverseQ, &res);

    // Write new rotated coordinates back to the point
    //(*point)[0] = res.v[0];
    //(*point)[1] = res.v[1];
    //(*point)[2] = res.v[2];
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
    //Vec3D tmp = axis;
    //tmp[0] = axis[0];
    //tmp[1] = axis[1];
    //tmp[2] = axis[2];
    q.s = cos (angle/2.0);
    //multiplyVectorScalar_intern (axis, sin(angle/2.0), &tmp);
    q.v = multiplyVectorScalar(axis, sin(angle/2.0));
    //q.v[0] = tmp[0];
    //q.v[1] = tmp[1];
    //q.v[2] = tmp[2];

    rotatePointWithQuaternion(q, point);

}
