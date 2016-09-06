#ifndef __LOGIC_H__
#define __LOGIC_H__
/**
 * @file
 * logic stuff
 *
 * @author Maurice Tollmien
 */

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

/* ------- BERECHNUNGEN ------- */
void moveCamera(int x, int y);
void zoomCamera(int x, int y);
/* ------- INIT ------- */

void initGame ();



#endif
