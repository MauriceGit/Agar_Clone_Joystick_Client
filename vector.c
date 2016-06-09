/**
 * @file
 * Hier ist die Datenhaltung und Programmlogik
 *
 * @author Maurice Tollmien
 */

#ifdef _WIN32
#include <windows.h>
#endif

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* ---- Eigene Header einbinden ---- */
#include "types.h"
#include "vector.h"


/**
 * Printet einen Vector aus.
 */
void printVector (Vec3D a)
{
    int i;
    printf("printVector:\n");
    printf("[%.1f/%.1f/%.1f]\n", a.x, a.y, a.z);
}

/**
 * Konvertierungsfunktion,
 * wandelt drei Koordinaten in den dazugehörigen Vektor um
 * @param x
 * @param y
 * @param z
 */
Vec3D toVector3D(GLfloat x, GLfloat y, GLfloat z)
{
    Vec3D res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}

/**
 * Berechnet die Länge es Vektors
 * @param v
 *@return float
 */
float vectorLength3D(Vec3D vector)
{
  return sqrt((vector.x*vector.x)+
              (vector.y*vector.y)+
              (vector.z*vector.z));
}

/**
 * Normiert eine Vektor
 * @param v der zu normierende Vektor
 * @return der normierte Vektor
 */
Vec3D normVector3D(Vec3D vector)
{
    float l = vectorLength3D(vector);
    if (l >= .00001f)
        return toVector3D(vector.x/l, vector.y/l, vector.z/l);
    return vector;
}

/**
 * Berechnet das Kreuzprodukt zweier Vektoren
 * @param
 * @param
 * @return
 */
Vec3D crossProduct3D(Vec3D a, Vec3D b)
{
    Vec3D product = toVector3D((a.x*b.z - a.z*b.y),
                      (a.z*b.x - a.x*b.z),
                      (a.x*b.y - a.y*b.x));
    return product;
}

/**
 * Multipliziert einen Vektor mit einem Skalar.
 */
Vec3D multiplyVectorScalar (Vec3D a, double s)
{
    Vec3D res;
    res.x = a.x * s;
    res.y = a.y * s;
    res.z = a.z * s;
    return res;
}

double scalarProduct (Vec3D a, Vec3D b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

/**
 * Zieht b von a ab, also: a-b
 */
Vec3D subtractVectorVector (Vec3D a, Vec3D b)
{
    Vec3D res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

/**
 * Teilt den Vector a durch s.
 */
Vec3D divideVectorScalar (Vec3D a, double s)
{
    return multiplyVectorScalar(a, 1.0/s);
}

/**
 * Addiert a und b und schreibt das Ergebnis in res.
 */
Vec3D addVectorVector (Vec3D a, Vec3D b)
{
    Vec3D res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

/**
 * Wandelt eine Zahl (Grad) in Radiant.
 * deg muss im Wertebereich 0..359 liegen!
 */
double degToRad (double deg)
{
    return deg*PI/180.0;
}

/**
 * Wandelt eine Zahl (Radiant) in Grad um.
 * deg muss im Wertebereich 0..PI liegen!
 */
double radToDeg (double rad)
{
    return rad*180.0/PI;
}

/**
 * Berechnet den Winkel zwischen zwei Vektoren und gibt das Ergebnis in
 * ° zurück (nicht radiant!).
 */
double angleVectorVector (Vec3D a, Vec3D b)
{
    return radToDeg (acos (scalarProduct(a, b) / (vectorLength3D(a)*vectorLength3D(b))));
}
















