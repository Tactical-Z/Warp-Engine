#pragma once

#include "VkBuffer.h"
#include "Vertex.h"

typedef struct{

    int mID;
    Vertex* mVertices;
    unsigned int mVertexCount;
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;

    Index* mIndices;
    unsigned int mIndexCount;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;

} MeshComponent;

typedef struct{

    int mID;
    vec3 mPosition;
    vec3 mRotation;
    vec3 mScale;

} TransformComponent;