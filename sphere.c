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
#include "vector.h"



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
    Vec3D point01 = addVectorVector(v0, multiplyVectorScalar(edge01, u));
    Vec3D point32 = addVectorVector(v3, multiplyVectorScalar(edge32, u));
    Vec3D diff = multiplyVectorScalar(subtractVectorVector(point32, point01), v);
    return addVectorVector(point01, diff);
}

void createSurfaceVertices(int numSubdivisions, Vec3D v0, Vec3D v1, Vec3D v2, Vec3D v3, Vec3D* data) {
    Vec3D edge01 = subtractVectorVector(v1, v0);
    Vec3D edge32 = subtractVectorVector(v2, v3);
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

            *((Vec3D*)&data[(vertexIndex + 0)*stride]) = p0;
            *((Vec3D*)&data[(vertexIndex + 1)*stride]) = p1;
            *((Vec3D*)&data[(vertexIndex + 2)*stride]) = p2;

            *((Vec3D*)&data[(vertexIndex + 3)*stride]) = p0;
            *((Vec3D*)&data[(vertexIndex + 4)*stride]) = p2;
            *((Vec3D*)&data[(vertexIndex + 5)*stride]) = p3;

            Vec3D sp10 = subtractVectorVector(p0, p1);
            Vec3D sp12 = subtractVectorVector(p2, p1);
            Vec3D sp30 = subtractVectorVector(p0, p3);
            Vec3D sp32 = subtractVectorVector(p2, p3);

            //Vec3D n1 = mlUnitCross(sp12, sp10);
            Vec3D n1 = crossProduct3D(sp12, sp10);
            *((Vec3D*)&data[(vertexIndex + 0)*stride + sizeof(Vec3D)]) = n1;
            *((Vec3D*)&data[(vertexIndex + 1)*stride + sizeof(Vec3D)]) = n1;
            *((Vec3D*)&data[(vertexIndex + 2)*stride + sizeof(Vec3D)]) = n1;

            //Vec3D n2 = mlUnitCross(sp30, sp32);
            Vec3D n2 = crossProduct3D(sp30, sp32);
            *((Vec3D*)&data[(vertexIndex + 3)*stride + sizeof(Vec3D)]) = n2;
            *((Vec3D*)&data[(vertexIndex + 4)*stride + sizeof(Vec3D)]) = n2;
            *((Vec3D*)&data[(vertexIndex + 5)*stride + sizeof(Vec3D)]) = n2;

            vertexIndex += 6;
        }
    }
}

void createUnitCubeVertices(int numSubdivisions, Vec3D* data) {
    int byteSizePerSide = numSubdivisions*numSubdivisions*6*(sizeof(Vec3D) + sizeof(Vec3D));
    createSurfaceVertices(numSubdivisions, toVector3D(-1, -1,  1), toVector3D( 1, -1,  1), toVector3D( 1,  1,  1), toVector3D(-1,  1,  1), data + 0*byteSizePerSide); // Front
    createSurfaceVertices(numSubdivisions, toVector3D(-1, -1, -1), toVector3D(-1, -1,  1), toVector3D(-1,  1,  1), toVector3D(-1,  1, -1), data + 1*byteSizePerSide); // Left
    createSurfaceVertices(numSubdivisions, toVector3D( 1, -1,  1), toVector3D( 1, -1, -1), toVector3D( 1,  1, -1), toVector3D( 1,  1,  1), data + 2*byteSizePerSide); // Right
    createSurfaceVertices(numSubdivisions, toVector3D(-1,  1,  1), toVector3D( 1,  1,  1), toVector3D( 1,  1, -1), toVector3D(-1,  1, -1), data + 3*byteSizePerSide); // Top
    createSurfaceVertices(numSubdivisions, toVector3D(-1, -1, -1), toVector3D( 1, -1, -1), toVector3D( 1, -1,  1), toVector3D(-1, -1,  1), data + 4*byteSizePerSide); // Bottom
    createSurfaceVertices(numSubdivisions, toVector3D(-1,  1, -1), toVector3D( 1,  1, -1), toVector3D( 1, -1, -1), toVector3D(-1, -1, -1), data + 5*byteSizePerSide); // Back
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
        Vec3D c = *(Vec3D*)&data[i * byteSizeVertex];

        // Spherical position
        Vec3D s;
        s.x = c.x * sqrt(1.0f - (c.y * c.y) / 2.0f - (c.z * c.z) / 2.0f + (c.y * c.y * c.z * c.z) / 3.0f);
        s.y = c.y * sqrt(1.0f - (c.z * c.z) / 2.0f - (c.x * c.x) / 2.0f + (c.z * c.z * c.x * c.x) / 3.0f);
        s.z = c.z * sqrt(1.0f - (c.x * c.x) / 2.0f - (c.y * c.y) / 2.0f + (c.x * c.x * c.y * c.y) / 3.0f);

        *((Vec3D*)&data[i * byteSizeVertex]) = s;

        // Die Normale ist gleich der Position
        *((Vec3D*)&data[i * byteSizeVertex + sizeof(Vec3D)]) = s;
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

