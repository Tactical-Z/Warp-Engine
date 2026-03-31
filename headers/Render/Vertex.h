#pragma once

#include "cglm.h"

#define NUM_VERTEX_ATTRIBUTES 3
typedef struct Vertex{
    vec2 mPosition;
    vec3 mColor;
    vec2 mUV;
} Vertex;

// VkVertexInputAttributeDescription* GetAttributeDescriptions() is the binding function for shader
// located in VkGraphicsPipeline

typedef unsigned int Index;

