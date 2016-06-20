/**
 * Logic regarding camera movements controlled by a joystick.
 *
 * @author Maurice Tollmien. Github: MauriceGit
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* ---- Eigene Header einbinden ---- */
#include "joystickCamera.h"
#include "vector.h"
#include "types.h"
#include "hmd.h"
#include "quaternions.h"

/** Kameraposition */
Vec3D G_JoyUpVector;
Vec3D G_JoyViewVector;
Vec3D G_JoyCameraTranslation;
Vec3D G_JoyCameraPosition;

/* ------- GETTER / SETTER ------- */

/**
 * Gibt die Kamerakoordinate zurück.
 */
double getJoyCameraPosition (int axis)
{
    switch (axis) {
        case 0: return G_JoyCameraPosition.x;
        case 1: return G_JoyCameraPosition.y;
        case 2: return G_JoyCameraPosition.z;
        default: return 0.0;
    }
}

double getJoyUp(int axis) {
    switch (axis) {
        case 0: return G_JoyUpVector.x;
        case 1: return G_JoyUpVector.y;
        case 2: return G_JoyUpVector.z;
        default: return 0.0;
    }
}

double getJoyCenter(int axis) {
    switch (axis) {
        case 0: return G_JoyCameraPosition.x + G_JoyViewVector.x;
        case 1: return G_JoyCameraPosition.y + G_JoyViewVector.y;
        case 2: return G_JoyCameraPosition.z + G_JoyViewVector.z;
        default: return 0.0;
    }
}

/* ------- BERECHNUNGEN ------- */

/**
 * Berechnet alle Funktionen, die vom interval abhängig sind
 * @param interval - Zeit
 */
void calcJoyCameraMovement ()
{

    handleHMDEvent();

    Quaternion q = getQuaternion();

    rotatePointWithQuaternion(q, &G_JoyViewVector);
    rotatePointWithQuaternion(q, &G_JoyUpVector);

    double forwardTranslation = -getTranslationAxisValue(4) / 500000.0;
    G_JoyViewVector = normVector3D(G_JoyViewVector);

    G_JoyCameraTranslation = addVectorVector(G_JoyCameraTranslation, multiplyVectorScalar(G_JoyViewVector, forwardTranslation));

    //G_JoyCameraTranslation[0] += G_JoyViewVector[0] * forwardTranslation;
    //G_JoyCameraTranslation[1] += G_JoyViewVector[1] * forwardTranslation;
    //G_JoyCameraTranslation[2] += G_JoyViewVector[2] * forwardTranslation;

    Vec3D sideDirection = crossProduct3D(G_JoyViewVector, G_JoyUpVector);
    sideDirection = normVector3D(sideDirection);
    double sideTranslation = getTranslationAxisValue(3) / 500000.0;

    G_JoyCameraTranslation = addVectorVector(G_JoyCameraTranslation, multiplyVectorScalar(sideDirection, sideTranslation));
    //G_JoyCameraTranslation[0] += sideDirection[0] * sideTranslation;
    //G_JoyCameraTranslation[1] += sideDirection[1] * sideTranslation;
    //G_JoyCameraTranslation[2] += sideDirection[2] * sideTranslation;

    Vec3D basePos = {.x=CAMERA_X, .y=CAMERA_Y, .z=CAMERA_Z};
    G_JoyCameraPosition = addVectorVector(basePos, G_JoyCameraTranslation);
    //G_JoyCameraPosition[0] = CAMERA_X + G_JoyCameraTranslation[0];
    //G_JoyCameraPosition[1] = CAMERA_Y + G_JoyCameraTranslation[1];
    //G_JoyCameraPosition[2] = CAMERA_Z + G_JoyCameraTranslation[2];


}

/* ------- INIT ------- */

/**
 * Hier findet die komplette Initialisierung des kompletten SPIEeles statt.
 * Inklusive der Datenhaltung und des SPIEelfeldes.
 */
void initJoyCamera ()
{
    G_JoyUpVector = {.x=0, .y=1, .z=0};
    G_JoyViewVector = {.x=-CAMERA_X, .y=-CAMERA_Y, .z=-CAMERA_Z};
    G_JoyViewVector = normVector3D(G_JoyViewVector);

    G_JoyCameraTranslation = {.x=0, .y=0, .z=0};
    G_JoyCameraPosition = {.x=CAMERA_X, .y=CAMERA_Y, .z=CAMERA_Z};

    if (!initializeHMD()) {
        printf("ERROR: hmd could not be initialized.\n");
    }
}

