

#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "vector.h"

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
//Vec3D bilinearPosition(Vec3D v0, Vec3D v3, Vec3D edge01, Vec3D edge32, float u, float v);
//void createSurfaceVertices(int numSubdivisions, Vec3D v0, Vec3D v1, Vec3D v2, Vec3D v3, GLvoid* data);
//void createUnitCubeVertices(int numSubdivisions, GLvoid* data);
Geometry createSurface(int numSubdivisions, Vec3D v0, Vec3D v1, Vec3D v2, Vec3D v3);
Geometry createUnitCube(int numSubdivisions);
Geometry createUnitSphere(int numSubdivisions);


#endif
