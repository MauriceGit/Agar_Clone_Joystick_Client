

#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "mtVector.h"

/**
 * Interface for spheres.
 *
 * @author Henk
 */

typedef struct {
    GLuint arrayBuffer;
    GLuint vertexArrayObject;
    int numVertices;
} Geometry;

//void rendering_setAttributePointer(GLuint vertexArrayObject, GLuint buffer, GLint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, unsigned offset);
//MTVec3D bilinearPosition(MTVec3D v0, MTVec3D v3, MTVec3D edge01, MTVec3D edge32, float u, float v);
//void createSurfaceVertices(int numSubdivisions, MTVec3D v0, MTVec3D v1, MTVec3D v2, MTVec3D v3, GLvoid* data);
//void createUnitCubeVertices(int numSubdivisions, GLvoid* data);
Geometry createSurface(int numSubdivisions, MTVec3D v0, MTVec3D v1, MTVec3D v2, MTVec3D v3);
Geometry createUnitCube(int numSubdivisions);
Geometry createUnitSphere(int numSubdivisions);


#endif
