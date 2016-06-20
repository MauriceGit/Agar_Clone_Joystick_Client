

/* - System Header einbinden */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* - eigene Header einbinden */
#include "hmd.h"
#include "quaternions.h"
#include "types.h"

#define NORM_PITCH 1.188

short G_A = 0, G_B = 0;

/**
 * Rechnet ein Einheits-Quaternion in eine Rotationsmatrix um.
 * @param q das Quaternion
 * @param m die Rotationsmatrix
 *
 */
static void quaternionToMatrix2( float q[4], float m[3][3] )
{
   m[0][0] = q[3] * q[3] + q[0] * q[0] - q[1] * q[1] - q[2] * q[2];
   m[0][1] = 2 * (q[0] * q[1] + q[2] * q[3]);
   m[0][2] = 2 * (q[0] * q[2] - q[1] * q[3]);

   m[1][0] = 2 * (q[0] * q[1] - q[2] * q[3]);
   m[1][1] = q[3] * q[3] - q[0] * q[0] + q[1] * q[1] - q[2] * q[2];
   m[1][2] = 2 * (q[1] * q[2] + q[0] * q[3]);

   m[2][0] = 2 * (q[0] * q[2] + q[1] * q[3]);
   m[2][1] = 2 * (q[1] * q[2] - q[0] * q[3]);
   m[2][2] = q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2];
}

/**
 * Rechnet ein Einheits-Quaternion in eine Rotationsmatrix um.
 * @param q das Quaternion
 * @param m die Rotationsmatrix
 *
 */
 static void quaternionToMatrix (Quaternion q, float m[3][3])
 {
    /* http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/ */
    /**
    * w = 0; x = 1; y = 2; z = 3.
    */

    // This is correct, but we have to change q[1] to q.v.y for example!
    /*m[0][0] = 1 - 2 * q[2] * q[2] - 2 * q[3] * q[3];
    m[0][1] = 2 * q[1] * q[2] - 2 * q[3] * q[0];
    m[0][2] = 2 * q[1] * q[3] + 2 * q[2] * q[0];

    m[1][0] = 2 * q[1] * q[2] + 2 * q[3] * q[0];
    m[1][1] = 1 - 2 * q[1] * q[1] - 2 * q[3] * q[3];
    m[1][2] = 2 * q[2] * q[3] - 2 * q[1] * q[0];

    m[2][0] = 2 * q[1] * q[3] - 2 * q[2] * q[0];
    m[2][1] = 2 * q[2] * q[3] + 2 * q[1] * q[0];
    m[2][2] = 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2];*/
 }

static float norm(float i) {
    float k = (1 + 1)/(NORM_PITCH+NORM_PITCH);

    return (i + NORM_PITCH) * k - 1;

}

/**
 * Errechnet basierend auf eine Rotationsmatrix
 * Roll-, Pitch- und Yawwinkel.
 * @param m Rotationsmatrix
 * @param ypr YawPitchRoll
 */
static void matrixToYPR( float m[3][3], float ypr[3] )
{
   ypr[0] = atan2( m[0][1], m[0][0] );


   ypr[1] = asin( norm(m[0][2]) );


   /*printf("m: %f\n", m[0][2]);*/
   ypr[2] = -atan2( m[1][2], m[2][2] );
}

float posToAngle(short pos) {
    return pos / 5000000.0;
}

/**
 * Liefert das Quaternion vom HMD normalisiert zurück.
 */
Quaternion getQuaternion() {
    short a,b;
    Quaternion q, qA, qB, qRes;

    /* im Fehlerfall gib 0 zurück. */
    if (!(
         getAxisValue(0, &a)
      && getAxisValue(1, &b)
      )) {
          printf("ERROR reading an axis value!\n");
          return q;
    }

    Vec3D vB = {.x=1, .y=0, .z=0};
    qB = createQuaternion(vB, posToAngle(b));

    Vec3D vA = {.x=0, .y=1, .z=0};
    qA = createQuaternion(vA, posToAngle(a));

    G_A = a;
    G_B = b;

    addQuaternionQuaternion(&qA, &qB, &qRes);

    normQuaternion(&qRes);
    return qRes;
}

short getTranslationAxisValue(int axis) {
    if (axis == 3 || axis == 4) {
        short v;
        if (!getAxisValue(axis, &v)) {
            printf("ERROR reading a translation axis value!\n");
            return 0;
        }
        return v;
    }
    printf("ERROR wrong axis value\n");
    return 0;
}

/**
 * Initialisiert/Überprüft die Verbindung mit dem HMD.
 * @return 1 = okay; 0 = fehler
 */
int initializeHMD() {
    if  (!startDeviceConnection(JOY_DEV)) {
        return 0;
    }

    /*if (!setCalibrationCoefficients(24617, 40917, 21844, 21844, 1, 255)) {
        return 0;
    }*/

    return 1;
}


/**
 * Liefert die Roll- Pitch- und Yaw-Winkel
 * @param ypr YawPitchRoll
 */
int getYPR(float ypr[3]) {
    float R[3][3];

    Quaternion q = getQuaternion();

    quaternionToMatrix(q, R);
    matrixToYPR(R, ypr);

    ypr[0] = ypr[0] / HMD_PI * 180;
    ypr[1] = ypr[1] / HMD_PI * 180;
    ypr[2] = ypr[2] / HMD_PI * 180;

    return 1;
}

/**
 * Beendet die Verbindung zum HMD.
 */
int closeHMD() {
    return endDeviceConnection();
}

/**
 * Liest die Events von dem HMD aus der Treiber-Queue.
 * Sollte kontinuierlich ausgeführt werden.
 */
void handleHMDEvent() {
    handleJoystickEvents();
}

/*
 * MODUL-TEST
int main(void) {

    float ypr[3];
    initializeHMD();

    while (1) {
        if (!getYPR(ypr)) {
            printf("idiot!\n");
            exit(1);
        }
        printf("yaw: %f, pitch: %f, roll: %f\n", ypr[0], ypr[1], ypr[2]);
    }
}
*/
