/*
 * Header file for shape mesh creation
 */

#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

struct GLMesh {
    GLuint vao;
    GLuint vbo[3];
    GLuint nIndices;
    GLuint nVertices;
    GLuint ebo;
};

void setTorusCoords(double majorRadius, double minorRadius, int majorSegments, int minorSegments,
    int majorIndex, int minorIndex, GLfloat* vertices, GLfloat* normals, GLfloat* uv);

void createTorus(GLMesh& mesh, double majorRadius, double minorRadius,
    int majorSegments, int minorSegments);

void createHalfSphere(GLMesh& mesh, double radius, int segments);

void createFullSphere(GLMesh& mesh, double radius, int segments);

void createCylinder(GLMesh& mesh, float radius, int numSlices, float height);

void createPlane(GLMesh& mesh, float width, float depth);

void createCube(GLMesh& mesh, GLfloat width, GLfloat height, GLfloat depth);

void UDestroyMesh(GLMesh& mesh);


#endif // MESH_H