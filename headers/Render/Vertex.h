#pragma once

#include "cglm.h"

#define NUM_VERTEX_ATTRIBUTES 2

typedef struct Vertex{
    vec2 mPosition;
    vec3 mColor;
} Vertex;

#define NUM_VERTICES 3
static const Vertex testVertices[] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};