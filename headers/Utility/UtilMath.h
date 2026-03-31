#pragma once

#include "HDF5Reader.h"
#include "cglm.h"

#define CLAMP(x,lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

uint8_t* GenerateMagnitudeHeatmap(VectorField field);

uint8_t* GenerateVorticityHeatmap(VectorField field);