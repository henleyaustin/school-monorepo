/*
 * Code for shape mesh creation
 *
 */

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shapes.h"


using namespace std;

// Constants for Pi and 2*Pi (Tau)
const GLfloat PI = 3.1415926535897932384626433832795f;
const GLfloat TAU = 2 * PI;

/*
 * Partner method for torus to select coordinates
 *
 */
void setTorusCoords(double majorRadius, double minorRadius, int majorSegments, int minorSegments,
    int majorIndex, int minorIndex, GLfloat* vertices, GLfloat* normals, GLfloat* uv)
{
    // Wrap the indices at the "seam" of the torus
    if (majorIndex >= majorSegments) {
        majorIndex = 0;
    }
    if (minorIndex >= minorSegments) {
        minorIndex = 0;
    }

    // Calculate the major and minor angles based on the segment count and indices
    GLfloat majorAngle = majorIndex * TAU / majorSegments;
    GLfloat minorAngle = minorIndex * TAU / minorSegments;

    // Using the major and minor angles, calculate the x, y, and z coordinates for the vertex
    GLfloat x = (majorRadius + minorRadius * cos(majorAngle)) * cos(minorAngle);
    GLfloat y = (majorRadius + minorRadius * cos(majorAngle)) * sin(minorAngle);
    GLfloat z = minorRadius * sin(majorAngle);

    // Calculate the normal vectors for the vertex, which are important for lighting calculations
    GLfloat nx = cos(minorAngle) * cos(majorAngle);
    GLfloat ny = sin(minorAngle) * cos(majorAngle);
    GLfloat nz = sin(majorAngle);

    // Store the vertex coordinates in the vertices array
    vertices[0] = x;
    vertices[1] = y;
    vertices[2] = z;

    // Store the normal vectors in the normals array
    normals[0] = nx;
    normals[1] = ny;
    normals[2] = nz;

    // Compute the UV texture coordinates for the vertex
    uv[0] = majorIndex / (double)majorSegments;
    uv[1] = minorIndex / (double)minorSegments;
}

/*
 * Function to create torus mesh
 *
 * Calculates vertices
 * Binds vao for attributes and vbo for vertex data
 *
 * 6/2 : Added UV coordinates - sourced from SNHU tutorial content
 * 6/8 : Added Normal coordinates - sourced from SNHU tutorial content
 * 6/9 - Removed color parameter (Not necessary/ Refactoring)
 *
 */
void createTorus(GLMesh& mesh, double majorRadius, double minorRadius,
    int majorSegments, int minorSegments) {

    // The total number of vertices in the torus is based on the major and minor segments
    int vertexCount = majorSegments * minorSegments * 6;

    // Allocate memory for the arrays of vertices, normals and texture coordinates (UVs)
    GLfloat* vertices = (GLfloat*)malloc(vertexCount * 3 * sizeof(GLfloat));
    GLfloat* normals = (GLfloat*)malloc(vertexCount * 3 * sizeof(GLfloat)); // Allocate memory for normals
    GLfloat* uv = (GLfloat*)malloc(vertexCount * 2 * sizeof(GLfloat));

    int vertexIndex = 0;
    int uvIndex = 0;

    // Iterate over each segment of the torus, generating the corresponding vertices, normals, and UVs
    for (int majorIndex = 0; majorIndex < majorSegments; majorIndex++) {
        for (int minorIndex = 0; minorIndex < minorSegments; minorIndex++) {

            // Call the setTorusCoords function for each of the six vertices of the current segment
            // Pass in the corresponding sub-array of the vertices, normals, and UVs arrays for each vertex
            setTorusCoords(majorRadius, minorRadius, majorSegments, minorSegments, majorIndex, minorIndex,
                vertices + vertexIndex, normals + vertexIndex, uv + uvIndex);

            setTorusCoords(majorRadius, minorRadius, majorSegments, minorSegments, majorIndex + 1, minorIndex,
                vertices + vertexIndex + 3, normals + vertexIndex + 3, uv + uvIndex + 2);

            setTorusCoords(majorRadius, minorRadius, majorSegments, minorSegments, majorIndex, minorIndex + 1,
                vertices + vertexIndex + 6, normals + vertexIndex + 6, uv + uvIndex + 4);

            setTorusCoords(majorRadius, minorRadius, majorSegments, minorSegments, majorIndex, minorIndex + 1,
                vertices + vertexIndex + 9, normals + vertexIndex + 9, uv + uvIndex + 6);

            setTorusCoords(majorRadius, minorRadius, majorSegments, minorSegments, majorIndex + 1, minorIndex,
                vertices + vertexIndex + 12, normals + vertexIndex + 12, uv + uvIndex + 8);

            setTorusCoords(majorRadius, minorRadius, majorSegments, minorSegments, majorIndex + 1, minorIndex + 1,
                vertices + vertexIndex + 15, normals + vertexIndex + 15, uv + uvIndex + 10);

            // Move to the next set of vertices in the array
            vertexIndex += 18;
            uvIndex += 12;
        }
    }

    // After generating all vertices, normals, and UVs, you can use these data for your OpenGL drawing operations...

    // Generate and bind the vertex array object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generate and bind the vertex buffer objects (vbo)
    glGenBuffers(3, mesh.vbo); // Now three buffers, one for vertices, one for normals and one for UVs.

    // Bind and fill the vertex buffer for the position coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Bind and fill the normal buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Bind and fill the vertex buffer for the texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(GLfloat), uv, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);

    // Set the number of indices for rendering
    mesh.nIndices = vertexCount;

    // Free the allocated memory for vertices, normals, and texture coordinates
    free(vertices);
    free(normals);
    free(uv);
}




/*
 * Function to create half sphere mesh
 *
 * Calculates vertices and indices for half-sphere
 * Binds vao for attributes and vbo for vertex data
 * Binds ebo for index data
 *
 * Utilized indexed vertices to create shape
 *
 * 6/2 : Added UV coordinate generation
 *
 */
void createHalfSphere(GLMesh& mesh, double radius, int segments) {
    // Calculate the total number of vertices and indices required for the half sphere
    int vertexCount = (segments + 1) * (segments + 2) / 2;
    int indexCount = 6 * segments * segments / 2;

    // Create vectors to store the vertices, normals, and UV coordinates
    vector<GLfloat> vertices(vertexCount * 3);
    vector<GLfloat> normals(vertexCount * 3);
    vector<GLfloat> texCoords(vertexCount * 2); // New vector for UV coordinates
    vector<GLuint> indices(indexCount);

    int index = 0;

    // Loop through the latitude and longitude segments to generate the vertices, normals, and UV coordinates
    for (int i = 0; i <= segments / 2; ++i) {
        GLfloat theta = i * PI / segments;
        GLfloat sinTheta = sin(theta);
        GLfloat cosTheta = cos(theta);

        for (int j = 0; j <= segments; ++j) {
            GLfloat phi = j * 2 * PI / segments;
            GLfloat sinPhi = sin(phi);
            GLfloat cosPhi = cos(phi);

            int vertexIndex = (i * (segments + 1) + j) * 3;

            // Calculate the position coordinates of the vertex based on the spherical coordinates
            vertices[vertexIndex] = radius * sinTheta * cosPhi;
            vertices[vertexIndex + 1] = radius * sinTheta * sinPhi;
            vertices[vertexIndex + 2] = radius * cosTheta;

            // Calculate the normal vector for the vertex
            normals[vertexIndex] = sinTheta * cosPhi;
            normals[vertexIndex + 1] = sinTheta * sinPhi;
            normals[vertexIndex + 2] = cosTheta;

            // Adjusted UV coordinates calculation
            texCoords[(i * (segments + 1) + j) * 2] = phi / (2.0 * PI);
            texCoords[(i * (segments + 1) + j) * 2 + 1] = theta / PI;
        }
    }


    index = 0;

    // Generate the indices for rendering the triangles
    for (int i = 0; i < segments / 2; ++i) {
        for (int j = 0; j < segments; ++j) {
            int v1 = i * (segments + 1) + j;
            int v2 = v1 + 1;
            int v3 = (i + 1) * (segments + 1) + j;
            int v4 = v3 + 1;

            // Define the indices for two triangles forming a "square"
            indices[index++] = v1;
            indices[index++] = v2;
            indices[index++] = v3;
            indices[index++] = v2;
            indices[index++] = v4;
            indices[index++] = v3;
        }
    }

    // Generate and bind the vertex array object
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generate and bind the vertex buffer objects
    glGenBuffers(3, mesh.vbo); // Increase the number of vbo to 3

    // Bind and fill the vertex buffer for the position coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Bind and fill the vertex buffer for the normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Bind and fill the vertex buffer for the UV coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(GLfloat), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Generate and bind the element buffer object for indices
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Set the number of indices for rendering
    mesh.nIndices = indexCount;

    // Unbind the vertex array object
    glBindVertexArray(0);
}

/*
 * Function to create full sphere mesh
 *
 * Calculates vertices and indices for half-sphere
 * Binds vao for attributes and vbo for vertex data
 * Binds ebo for index data
 *
 * Utilized indexed vertices to create shape
 *
 * Utilized for light spheres
 *
 */
void createFullSphere(GLMesh& mesh, double radius, int segments) {
    int vertexCount = (segments + 1) * (segments + 1);
    int indexCount = 6 * segments * segments;

    vector<GLfloat> vertices(vertexCount * 3);
    vector<GLfloat> normals(vertexCount * 3);
    vector<GLfloat> texCoords(vertexCount * 2);
    vector<GLuint> indices(indexCount);

    // Generate the vertices, normals, and UV coordinates
    for (int i = 0; i <= segments; ++i) {
        GLfloat theta = i * PI / segments;
        GLfloat sinTheta = sin(theta);
        GLfloat cosTheta = cos(theta);

        for (int j = 0; j <= segments; ++j) {
            GLfloat phi = j * 2 * PI / segments;
            GLfloat sinPhi = sin(phi);
            GLfloat cosPhi = cos(phi);

            int vertexIndex = (i * (segments + 1) + j) * 3;

            vertices[vertexIndex] = radius * sinTheta * cosPhi;
            vertices[vertexIndex + 1] = radius * sinTheta * sinPhi;
            vertices[vertexIndex + 2] = radius * cosTheta;

            normals[vertexIndex] = sinTheta * cosPhi;
            normals[vertexIndex + 1] = sinTheta * sinPhi;
            normals[vertexIndex + 2] = cosTheta;

            texCoords[(i * (segments + 1) + j) * 2] = phi / (2.0 * PI);
            texCoords[(i * (segments + 1) + j) * 2 + 1] = theta / PI;
        }
    }

    // Generate the indices for rendering the triangles
    for (int i = 0; i < segments; ++i) {
        for (int j = 0; j < segments; ++j) {
            int v1 = i * (segments + 1) + j;
            int v2 = v1 + 1;
            int v3 = (i + 1) * (segments + 1) + j;
            int v4 = v3 + 1;

            indices[i * 6 * segments + j * 6] = v1;
            indices[i * 6 * segments + j * 6 + 1] = v3;
            indices[i * 6 * segments + j * 6 + 2] = v2;
            indices[i * 6 * segments + j * 6 + 3] = v2;
            indices[i * 6 * segments + j * 6 + 4] = v3;
            indices[i * 6 * segments + j * 6 + 5] = v4;
        }
    }

    // Generate and bind the vertex array object
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generate and bind the vertex buffer objects
    glGenBuffers(3, mesh.vbo); // Increase the number of vbo to 3

    // Bind and fill the vertex buffer for the position coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Bind and fill the vertex buffer for the normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Bind and fill the vertex buffer for the UV coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(GLfloat), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Generate and bind the element buffer object for indices
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    mesh.nIndices = indexCount;

    // Unbind the vertex array object
    glBindVertexArray(0);
}

/*
 * Function to create cylinder mesh
 *
 * Calculates vertices and indices for cylinder
 * Binds vao for attributes and vbo for vertex data
 * Binds ebo for index data
 *
 * Utilized indexed vertices to create shape
 *
 * 6/9 - Removed color parameter (Not necessary/ Refactoring)
 *
 */
void createCylinder(GLMesh& mesh, float radius, int numSlices, float height)
{
    // Calculate the total number of vertices and indices required for the cylinder
    int vertexCount = (numSlices + 1) * 4; // 4 vertices for each slice (top side, bottom side, top cap, bottom cap)
    int indexCount = numSlices * 12; // 12 indices per slice (6 for the side, 3 for the top cap, 3 for the bottom cap)

    std::vector<GLfloat> vertices(vertexCount * 3);
    std::vector<GLfloat> normals(vertexCount * 3);
    std::vector<GLfloat> uvs(vertexCount * 2);
    std::vector<GLuint> indices(indexCount);

    float sectorStep = 2 * PI / numSlices;
    float sectorAngle;

    for (int i = 0; i <= numSlices; ++i)
    {
        sectorAngle = i * sectorStep;

        // Bottom side vertex
        vertices[i * 3] = radius * cos(sectorAngle);
        vertices[i * 3 + 1] = -height / 2.0f;
        vertices[i * 3 + 2] = radius * sin(sectorAngle);

        // Top side vertex
        vertices[(numSlices + 1 + i) * 3] = radius * cos(sectorAngle);
        vertices[(numSlices + 1 + i) * 3 + 1] = height / 2.0f;
        vertices[(numSlices + 1 + i) * 3 + 2] = radius * sin(sectorAngle);

        // Bottom cap vertex
        vertices[(2 * numSlices + 2 + i) * 3] = radius * cos(sectorAngle);
        vertices[(2 * numSlices + 2 + i) * 3 + 1] = -height / 2.0f;
        vertices[(2 * numSlices + 2 + i) * 3 + 2] = radius * sin(sectorAngle);

        // Top cap vertex
        vertices[(3 * numSlices + 3 + i) * 3] = radius * cos(sectorAngle);
        vertices[(3 * numSlices + 3 + i) * 3 + 1] = height / 2.0f;
        vertices[(3 * numSlices + 3 + i) * 3 + 2] = radius * sin(sectorAngle);

        // Side normals
        normals[i * 3] = cos(sectorAngle);
        normals[i * 3 + 1] = 0.0f;
        normals[i * 3 + 2] = sin(sectorAngle);

        normals[(numSlices + 1 + i) * 3] = cos(sectorAngle);
        normals[(numSlices + 1 + i) * 3 + 1] = 0.0f;
        normals[(numSlices + 1 + i) * 3 + 2] = sin(sectorAngle);

        // Bottom cap normals
        normals[(2 * numSlices + 2 + i) * 3] = 0.0f;
        normals[(2 * numSlices + 2 + i) * 3 + 1] = -1.0f;
        normals[(2 * numSlices + 2 + i) * 3 + 2] = 0.0f;

        // Top cap normals
        normals[(3 * numSlices + 3 + i) * 3] = 0.0f;
        normals[(3 * numSlices + 3 + i) * 3 + 1] = 1.0f;
        normals[(3 * numSlices + 3 + i) * 3 + 2] = 0.0f;

        // UVs - For simplicity, same UVs for side and cap vertices
        uvs[i * 2] = static_cast<float>(i) / numSlices; // u
        uvs[i * 2 + 1] = 0.0f; // v

        uvs[(numSlices + 1 + i) * 2] = static_cast<float>(i) / numSlices; // u
        uvs[(numSlices + 1 + i) * 2 + 1] = 1.0f; // v

        uvs[(2 * numSlices + 2 + i) * 2] = static_cast<float>(i) / numSlices; // u
        uvs[(2 * numSlices + 2 + i) * 2 + 1] = 0.5f; // v

        uvs[(3 * numSlices + 3 + i) * 2] = static_cast<float>(i) / numSlices; // u
        uvs[(3 * numSlices + 3 + i) * 2 + 1] = 0.5f; // v
    }

    // Generate the indices for rendering the cylinder
    int index = 0;
    for (int i = 0; i < numSlices; ++i)
    {
        int nextSlice = (i + 1) % (numSlices + 1);

        // Side face
        indices[index++] = i;
        indices[index++] = nextSlice;
        indices[index++] = numSlices + 1 + i;

        indices[index++] = nextSlice;
        indices[index++] = numSlices + 1 + nextSlice;
        indices[index++] = numSlices + 1 + i;

        // Bottom cap face
        indices[index++] = 2 * numSlices + 2 + i;
        indices[index++] = 2 * numSlices + 2 + nextSlice;
        indices[index++] = 4 * numSlices + 4; // center point

        // Top cap face
        indices[index++] = 3 * numSlices + 3 + i;
        indices[index++] = 3 * numSlices + 3 + nextSlice;
        indices[index++] = 4 * numSlices + 5; // center point
    }

    // Add the center points of the top and bottom circle
    vertices.push_back(0);
    vertices.push_back(-height / 2.0f);
    vertices.push_back(0);

    vertices.push_back(0);
    vertices.push_back(height / 2.0f);
    vertices.push_back(0);

    // Add normals for the center points
    normals.push_back(0);
    normals.push_back(-1);
    normals.push_back(0);

    normals.push_back(0);
    normals.push_back(1);
    normals.push_back(0);

    // Add UVs for the center points
    uvs.push_back(0.5f);
    uvs.push_back(0.5f);

    uvs.push_back(0.5f);
    uvs.push_back(0.5f);

    // Generate and bind the vertex array object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generate and bind the vertex buffer objects (VBO)
    glGenBuffers(3, mesh.vbo);

    // Bind and fill the vertex buffer for the position coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Bind and fill the vertex buffer for the normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Bind and fill the vertex buffer for the UV coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Generate and bind the element buffer object (EBO) for indices
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Set the number of indices for rendering
    mesh.nIndices = indices.size();

    // Unbind the VAO
    glBindVertexArray(0);
}


/*
 * Function to create plane
 *
 * Calculates vertices and indices for plane
 * Binds vao for attributes and vbo for vertex data
 * Binds ebo for index data
 *
 * Utilized indexed vertices to create flat item
 *
 * 6/2 : Added UV coordinates for textures
 *
 */
void createPlane(GLMesh& mesh, float width, float depth) {
    // Define the vertices for the plane
    GLfloat vertices[] = {
        // Vertex Coordinates        // Normals             // Texture Coordinates
        -width / 2, 0.0f, -depth / 2, 0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // Bottom-left
         width / 2, 0.0f, -depth / 2, 0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Bottom-right
         width / 2, 0.0f,  depth / 2, 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, // Top-right
        -width / 2, 0.0f,  depth / 2, 0.0f, 1.0f, 0.0f,    0.0f, 1.0f  // Top-left
    };

    // Define the indices for rendering the plane as triangles
    GLuint indices[] = {
        0, 1, 2,  // Triangle 1
        0, 2, 3   // Triangle 2
    };

    // Generate and bind the vertex array object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generate and bind the vertex buffer object (VBO) for the position, normals and texture coordinates
    glGenBuffers(1, &mesh.vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0); // Vertex coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // Texture coordinates
    glEnableVertexAttribArray(2);

    // Generate and bind the element buffer object (EBO) for indices
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the number of indices for rendering
    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);

    // Unbind the VAO
    glBindVertexArray(0);
}

/*
 * Function to create cube
 *
 * Calculates vertices and indices for plane
 * Binds vao for attributes and vbo for vertex data
 * Binds ebo for index data
 *
 * Utilized indexed vertices to create flat item
 *
 * 6/2 : Added UV coordinates for textures
 *
 */
void createCube(GLMesh& mesh, GLfloat width, GLfloat height, GLfloat depth)
{

    GLfloat halfWidth = width / 2.0f;
    GLfloat halfHeight = height / 2.0f;
    GLfloat halfDepth = depth / 2.0f;

    // Position and Color data
    GLfloat verts[] = {
        // Back Face
       -halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       -halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
       -halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

       // Front Face
       -halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       -halfWidth,  halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
       -halfWidth, -halfHeight,  halfDepth,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

       // Left Face
       -halfWidth,  halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
       -halfWidth,  halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
       -halfWidth, -halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -halfWidth, -halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       -halfWidth, -halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       -halfWidth,  halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

       // Right Face
        halfWidth,  halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        halfWidth,  halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        halfWidth, -halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        halfWidth, -halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        halfWidth, -halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        halfWidth,  halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // Top Face
       -halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
       -halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
       -halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,

       // Bottom Face
       -halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       -halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
       -halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
    };


    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);


    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
}

/*
 * Delete Mesh
 *
 */
void UDestroyMesh(GLMesh& mesh) {
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbo);
}