#ifndef __LOGIC_H__
#define __LOGIC_H__
#define __DEBUG_GL_H__
/**
 * @file
 * logic stuff
 *
 * @author Maurice Tollmien
 */
#include <GL/glu.h>

#include <stdio.h>
#include <stdarg.h>
#include "types.h"

/* eig. Funktionen */

/* ------- GETTER / SETTER ------- */
void setMouseEvent(MouseInterpretType state,int x, int y);
void setMouseState(MouseInterpretType state);
void setMouseCoord(int x, int y);
MouseInterpretType getMouseEvent(void);
double getCameraPosition (int axis);
void setCameraMovement(int x,int y);
void setCameraZoom(int x,int y);
void setKey (int key, int value);
int getFPS(void);

/* ------- BERECHNUNGEN ------- */
void moveCamera(int x, int y);
void zoomCamera(int x, int y);
void calcTimeRelatedStuff (double interval);
/* ------- INIT ------- */

void initGame ();



#endif
