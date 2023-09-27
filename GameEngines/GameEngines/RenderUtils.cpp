
#include "RenderUtils.h"

// Create a buffer of vertices
unsigned int GE::createVBO(const std::vector<Vertex>& vertices)
{
    unsigned int newBuffer;
    glCreateBuffers(1, &newBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, newBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), (void*)&vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    return newBuffer;
}

// Create a buffer of indices
unsigned int GE::createIBO(const std::vector<unsigned int>& indices)
{
    unsigned int newBuffer;
    glCreateBuffers(1, &newBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, newBuffer);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), (void*)&indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    return newBuffer;
}