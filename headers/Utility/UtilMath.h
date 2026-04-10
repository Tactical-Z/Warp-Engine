#pragma once

#include "HDF5Reader.h"
#include "cglm.h"

#define CLAMP(x,lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

uint8_t* GenerateMagnitudeHeatmap(VectorField field);

uint8_t* GenerateVorticityHeatmap(VectorField field);

typedef enum {
    PATH_LINE,
    STREAM_LINE
} FieldlineType;

void SampleVectorField(VectorField* vf, float x, float y, vec2 out);

size_t IntegrateEuler(VectorField* _vf, vec2 _start, float _stepSize, int _maxSteps, vec2* _outPoints, FieldlineType _flt);

size_t IntegrateRK4(VectorField* _vf, vec2 _start, float _h, int _maxSteps, vec2* _outPoints, FieldlineType _flt);

