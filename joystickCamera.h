#ifndef __JOYSTICKCAMERA_H__
#define __JOYSTICKCAMERA_H__
/**
 * @author Maurice Tollmien. Github: MauriceGit
 */

#include <GL/glu.h>
#include <stdio.h>
#include <stdarg.h>
#include "types.h"

/* ------- GETTER / SETTER ------- */
double getJoyCameraPosition (int axis);
double getJoyUp(int axis);
double getJoyCenter(int axis);

/* ------- BERECHNUNGEN ------- */
void calcJoyCameraMovement();

void initJoyCamera();

#endif
