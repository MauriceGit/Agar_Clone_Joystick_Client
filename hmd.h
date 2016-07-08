/*
 * hmd.h
 *
 *  Author: Maurice Tollmien
 */

/* Joystickbehandlungsfunktionen */
#include "joystick.h"
#include "quaternions.h"

#define HMD_PI 3.141592654
#define JOY_DEV "/dev/input/js1"

/**
 * Liefert das Quaternion vom HMD normalisiert zurück.
 */
Quaternion getQuaternion(Vec3D jawAxis, Vec3D turnAxis, double minJawAngle, double maxJawAngle, double factor);

/**
 * Gibt den absoluten Wert, den der Joystick liefert.
 */
short getTranslationAxisValue(int axis);

/**
 * Initialisiert/Überprüft die Verbindung mit dem HMD.
 * @return 1 = okay; 0 = fehler
 */
int initializeHMD();

/**
 * Liefert die Roll- Pitch- und Yaw-Winkel
 * @param ypr YawPitchRoll
 */
int getYPR(float ypr[3]);

/**
 * Beendet die Verbindung zum HMD.
 */
int closeHMD();

/**
 * Liest die Events von dem HMD aus der Treiber-Queue.
 * Sollte kontinuierlich ausgeführt werden.
 */
void handleHMDEvent();
