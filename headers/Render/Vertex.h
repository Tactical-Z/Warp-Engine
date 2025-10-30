#pragma once

#include "cglm.h"

#define NUM_VERTEX_ATTRIBUTES 2

typedef struct Vertex{
    vec2 mPosition;
    vec3 mColor;
} Vertex;

typedef struct UniformBufferObject {
    __attribute__((aligned(16))) mat4 model;
    __attribute__((aligned(16))) mat4 view;
    __attribute__((aligned(16))) mat4 proj;
}UniformBufferObject;

#define NUM_VERTICES 4
static const Vertex testVertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

static const uint16_t testIndices[] = {
    0, 1, 2, 2, 3, 0
};