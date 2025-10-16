#pragma once

#include "cglm.h"

#define NUM_VERTEX_ATTRIBUTES 2
#define NUM
typedef struct Vertex{
    vec2 mPosition;
    vec3 mColor;
} Vertex;

static const Vertex testVertices[] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};