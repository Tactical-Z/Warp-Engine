#pragma once

#include "cglm.h"

#define CLAMP(x,lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

uint8_t* GenerateMagnitudeHeatmap(vec2* field, size_t width, size_t height);

uint8_t* GenerateVorticityHeatmap(vec2* field, size_t width, size_t height);