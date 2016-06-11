#ifndef __TYPES_H__
#define __TYPES_H__

/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
 *
 * @author Maurice Tollmien
 */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* ---- Eigene Konstanten */

#define CAMERA_X        -30.0
#define CAMERA_Y        600.0
#define CAMERA_Z        70.0
#define CAMERA_SPEED    15.0

#define CAMERA_MOVEMENT_SPEED   4.0
#define CAMERA_ZOOM_SPEED 10.0

#define E           2.71828183
#define PI             3.14159265
#define EPS     0.0001

/** Mausereignisse. */
enum e_MouseInterpretType
{ NONE, MOVE, ZOOM};
/** Datentyp fuer Mausereignisse. */
typedef enum e_MouseInterpretType MouseInterpretType;

/** Vektor im 3D-Raum */
typedef GLfloat CGVector3D[3];

typedef int Movement[3];


#endif
