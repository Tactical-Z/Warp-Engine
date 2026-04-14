#pragma once

#include "HDF5Reader.h"
#include "string.h"
#include "cglm.h"

#define FIELD_SCALE 50.0f


#define CLAMP(x,lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

uint8_t* GenerateMagnitudeHeatmap(VectorField field);

uint8_t* GenerateVorticityHeatmap(VectorField field);

typedef enum {
    PATH_LINE,
    STREAM_LINE
} FieldlineType;

void SampleVectorField(VectorField* vf, float x, float y, vec2 out);

size_t IntegrateEuler(VectorField* _vf, vec2 _start, float _stepSize, int _maxSteps, vec2* _outPoints, FieldlineType _flt);

size_t IntegrateRK4(
    VectorField* vf,
    vec2 start,
    float h,
    int maxSteps,
    vec2* outPoints,
    FieldlineType flt);
