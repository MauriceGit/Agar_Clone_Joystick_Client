/**
 * Some basic vector calculations.
 * All operations are prefixed with 'mt' to avoid name clashes and get a
 * attempt for a unique prefix.
 *
 * @author Maurice Tollmien
 */

#include <stdio.h>
//#include <stddef.h>
//#include <stdlib.h>
#include <math.h>

//#include "types.h"
#include <GL/gl.h>
#include "mtVector.h"


/**
 * Printet einen Vector aus.
 */
void mtPrintVector (Vec3D a)
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
Vec3D mtToVector3D(float x, float y, float z)
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
float mtVectorLength3D(Vec3D vector)
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
Vec3D mtNormVector3D(Vec3D vector)
{
    float l = mtVectorLength3D(vector);
    if (l >= .00001f)
        return mtToVector3D(vector.x/l, vector.y/l, vector.z/l);
    return vector;
}

/**
 * Berechnet das Kreuzprodukt zweier Vektoren
 * @param
 * @param
 * @return
 */
Vec3D mtCrossProduct3D(Vec3D a, Vec3D b)
{
    Vec3D product = mtToVector3D((a.x*b.z - a.z*b.y),
                      (a.z*b.x - a.x*b.z),
                      (a.x*b.y - a.y*b.x));
    return product;
}

/**
 * Multipliziert einen Vektor mit einem Skalar.
 */
Vec3D mtMultiplyVectorScalar (Vec3D a, double s)
{
    Vec3D res;
    res.x = a.x * s;
    res.y = a.y * s;
    res.z = a.z * s;
    return res;
}

double mtScalarProduct (Vec3D a, Vec3D b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

/**
 * Zieht b von a ab, also: a-b
 */
Vec3D mtSubtractVectorVector (Vec3D a, Vec3D b)
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
Vec3D mtDivideVectorScalar (Vec3D a, double s)
{
    return mtMultiplyVectorScalar(a, 1.0/s);
}

/**
 * Addiert a und b und schreibt das Ergebnis in res.
 */
Vec3D mtAddVectorVector (Vec3D a, Vec3D b)
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
double mtDegToRad (double deg)
{
    return deg*MT_PI/180.0;
}

/**
 * Wandelt eine Zahl (Radiant) in Grad um.
 * deg muss im Wertebereich 0..PI liegen!
 */
double mtRadToDeg (double rad)
{
    return rad*180.0/MT_PI;
}

/**
 * Berechnet den Winkel zwischen zwei Vektoren und gibt das Ergebnis in
 * ° zurück (nicht radiant!).
 */
double mtAngleVectorVector (Vec3D a, Vec3D b)
{
    return mtRadToDeg (acos (mtScalarProduct(a, b) / (mtVectorLength3D(a)*mtVectorLength3D(b))));
}
















