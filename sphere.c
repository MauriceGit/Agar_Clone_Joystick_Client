/**
 * Implementation of most relevant functions for the sphere.
 *
 * @author Henk
 */

/* ---- System Header ---- */
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>

/* ---- My Header ---- */
#include "sphere.h"
#include "mtVector.h"



void rendering_setAttributePointer(GLuint vertexArrayObject, GLuint buffer, GLint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, unsigned offset) {
    GLint previousVertexArrayObject;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVertexArrayObject);
    GLint previousArrayBuffer;
    glGetIntegerv(GL_ARRAY_BUFFER, &previousArrayBuffer);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, size, type, normalized, stride, (void*)(intptr_t)offset);

    glBindBuffer(GL_ARRAY_BUFFER, previousArrayBuffer);
    glBindVertexArray(previousVertexArrayObject);
}


Vec3D bilinearPosition(Vec3D v0, Vec3D v3, Vec3D edge01, Vec3D edge32, float u, float v) {
    Vec3D point01 = mtAddVectorVector(v0, mtMultiplyVectorScalar(edge01, u));
    Vec3D point32 = mtAddVectorVector(v3, mtMultiplyVectorScalar(edge32, u));
    Vec3D diff = mtMultiplyVectorScalar(mtSubtractVectorVector(point32, point01), v);
    return mtAddVectorVector(point01, diff);
}

void createSurfaceVertices(int numSubdivisions, Vec3D v0, Vec3D v1, Vec3D v2, Vec3D v3, Vec3D* data) {
    Vec3D edge01 = mtSubtractVectorVector(v1, v0);
    Vec3D edge32 = mtSubtractVectorVector(v2, v3);
    float xStep = 1.0f / numSubdivisions;
    float yStep = 1.0f / numSubdivisions;
    int stride = sizeof(Vec3D) + sizeof(Vec3D);
    int vertexIndex = 0;

    for (int x = 0; x < numSubdivisions; ++x) {
        for (int y = 0; y < numSubdivisions; ++y) {

            Vec3D p0 = bilinearPosition(v0, v3, edge01, edge32, (x + 0)*xStep, (y + 0)*yStep);
            Vec3D p1 = bilinearPosition(v0, v3, edge01, edge32, (x + 1)*xStep, (y + 0)*yStep);
            Vec3D p2 = bilinearPosition(v0, v3, edge01, edge32, (x + 1)*xStep, (y + 1)*yStep);
            Vec3D p3 = bilinearPosition(v0, v3, edge01, edge32, (x + 0)*xStep, (y + 1)*yStep);

// Hier habe ich mir die Position der Vec3Ds in byte berechnet. Da du jetzt einen Vec3D-Pointer hast musst du dir das anders berechnen.
// (2*, weil jeder Vertex aus einer Position und einer Normale besteht.)
// vertexIndex ist ein schlechter Name. Das ist der Index des Quads.
            data[2*(vertexIndex + 0)] = p0;
            data[2*(vertexIndex + 1)] = p1;
            data[2*(vertexIndex + 2)] = p2;

            data[2*(vertexIndex + 3)] = p0;
            data[2*(vertexIndex + 4)] = p2;
            data[2*(vertexIndex + 5)] = p3;

            Vec3D sp10 = mtSubtractVectorVector(p0, p1);
            Vec3D sp12 = mtSubtractVectorVector(p2, p1);
            Vec3D sp30 = mtSubtractVectorVector(p0, p3);
            Vec3D sp32 = mtSubtractVectorVector(p2, p3);

// Hier noch +1 rechnen, weil die Normale einen Vec3D weiter liegt.
            Vec3D n1 = mtCrossProduct3D(sp12, sp10);
            data[2*(vertexIndex + 0) + 1] = n1;
            data[2*(vertexIndex + 1) + 1] = n1;
            data[2*(vertexIndex + 2) + 1] = n1;

            Vec3D n2 = mtCrossProduct3D(sp30, sp32);
            data[2*(vertexIndex + 3) + 1] = n2;
            data[2*(vertexIndex + 4) + 1] = n2;
            data[2*(vertexIndex + 5) + 1] = n2;

            vertexIndex += 6;
        }
    }
}

void createUnitCubeVertices(int numSubdivisions, Vec3D* data) {
// Hier das offset jetzt auch nicht mehr auf byte-Basis, sondern auf basis von Vec3D
    int verticesPerSide = numSubdivisions*numSubdivisions*6;
    int vec3dsPerSide = 2*verticesPerSide;
    createSurfaceVertices(numSubdivisions, mtToVector3D(-1, -1,  1), mtToVector3D( 1, -1,  1), mtToVector3D( 1,  1,  1), mtToVector3D(-1,  1,  1), data + 0*vec3dsPerSide); // Front
    createSurfaceVertices(numSubdivisions, mtToVector3D(-1, -1, -1), mtToVector3D(-1, -1,  1), mtToVector3D(-1,  1,  1), mtToVector3D(-1,  1, -1), data + 1*vec3dsPerSide); // Left
    createSurfaceVertices(numSubdivisions, mtToVector3D( 1, -1,  1), mtToVector3D( 1, -1, -1), mtToVector3D( 1,  1, -1), mtToVector3D( 1,  1,  1), data + 2*vec3dsPerSide); // Right
    createSurfaceVertices(numSubdivisions, mtToVector3D(-1,  1,  1), mtToVector3D( 1,  1,  1), mtToVector3D( 1,  1, -1), mtToVector3D(-1,  1, -1), data + 3*vec3dsPerSide); // Top
    createSurfaceVertices(numSubdivisions, mtToVector3D(-1, -1, -1), mtToVector3D( 1, -1, -1), mtToVector3D( 1, -1,  1), mtToVector3D(-1, -1,  1), data + 4*vec3dsPerSide); // Bottom
    createSurfaceVertices(numSubdivisions, mtToVector3D(-1,  1, -1), mtToVector3D( 1,  1, -1), mtToVector3D( 1, -1, -1), mtToVector3D(-1, -1, -1), data + 5*vec3dsPerSide); // Back
}

Geometry createSurface(int numSubdivisions, Vec3D v0, Vec3D v1, Vec3D v2, Vec3D v3) {
    int numVertices = 6*numSubdivisions*numSubdivisions;

    int byteSizeVertex = sizeof(Vec3D) + sizeof(Vec3D);
    int byteSizeData = numVertices*byteSizeVertex;
    Vec3D* data = (Vec3D*)malloc(byteSizeData);

    createSurfaceVertices(numSubdivisions, v0, v1, v2, v3, data);

    Geometry geometry;
    geometry.numVertices = numVertices;

    glGenBuffers(1, &geometry.arrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.arrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, byteSizeData, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &geometry.vertexArrayObject);
    glBindVertexArray(geometry.vertexArrayObject);

    rendering_setAttributePointer(geometry.vertexArrayObject, geometry.arrayBuffer, 0, 3, GL_FLOAT, GL_FALSE, byteSizeVertex, 0);
    rendering_setAttributePointer(geometry.vertexArrayObject, geometry.arrayBuffer, 1, 3, GL_FLOAT, GL_TRUE , byteSizeVertex, sizeof(Vec3D));

    free(data);

    return geometry;
}






Geometry createUnitCube(int numSubdivisions) {
    int numVerticesPerSide = 6*numSubdivisions*numSubdivisions;
    int numVertices = 6*numVerticesPerSide;

    int byteSizeVertex = sizeof(Vec3D) + sizeof(Vec3D);
    int byteSizeData = numVertices*byteSizeVertex;
    Vec3D* data = (Vec3D*)malloc(byteSizeData);

    createUnitCubeVertices(numSubdivisions, data);

    Geometry geometry;
    geometry.numVertices = numVertices;

    glGenBuffers(1, &geometry.arrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.arrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, byteSizeData, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &geometry.vertexArrayObject);
    glBindVertexArray(geometry.vertexArrayObject);

    rendering_setAttributePointer(geometry.vertexArrayObject, geometry.arrayBuffer, 0, 3, GL_FLOAT, GL_FALSE, byteSizeVertex, 0);
    rendering_setAttributePointer(geometry.vertexArrayObject, geometry.arrayBuffer, 1, 3, GL_FLOAT, GL_TRUE , byteSizeVertex, sizeof(Vec3D));

    free(data);

    return geometry;
}

Geometry createUnitSphere(int numSubdivisions) {
    int numVerticesPerSide = 6*numSubdivisions*numSubdivisions;
    int numVertices = 6*numVerticesPerSide;

    int byteSizeVertex = sizeof(Vec3D) + sizeof(Vec3D);
    int byteSizeData = numVertices*byteSizeVertex;
    Vec3D* data = (Vec3D*)malloc(byteSizeData*1000);

    createUnitCubeVertices(numSubdivisions, data);

    for (int i = 0; i < numVertices; ++i) {
        // Cubical position
        Vec3D c = data[2*i];

        // Spherical position
        Vec3D s;
        s.x = c.x * sqrt(1.0f - (c.y * c.y) / 2.0f - (c.z * c.z) / 2.0f + (c.y * c.y * c.z * c.z) / 3.0f);
        s.y = c.y * sqrt(1.0f - (c.z * c.z) / 2.0f - (c.x * c.x) / 2.0f + (c.z * c.z * c.x * c.x) / 3.0f);
        s.z = c.z * sqrt(1.0f - (c.x * c.x) / 2.0f - (c.y * c.y) / 2.0f + (c.x * c.x * c.y * c.y) / 3.0f);

        data[2*i] = s;

        // Die Normale ist gleich der Position
        data[2*i + 1] = s;
    }

    Geometry geometry;
    geometry.numVertices = numVertices;

    glGenBuffers(1, &geometry.arrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.arrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, byteSizeData, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &geometry.vertexArrayObject);
    glBindVertexArray(geometry.vertexArrayObject);

    rendering_setAttributePointer(geometry.vertexArrayObject, geometry.arrayBuffer, 0, 3, GL_FLOAT, GL_FALSE, byteSizeVertex, 0);
    rendering_setAttributePointer(geometry.vertexArrayObject, geometry.arrayBuffer, 1, 3, GL_FLOAT, GL_TRUE , byteSizeVertex, sizeof(Vec3D));

    free(data);

    return geometry;
}

