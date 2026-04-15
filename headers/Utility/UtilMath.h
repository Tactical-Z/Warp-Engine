#pragma once

#include "HDF5Reader.h"
#include "string.h"
#include "cglm.h"

#define FIELD_SCALE 50.0f


#define CLAMP(x,lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

uint8_t* GenerateMagnitudeHeatmap(VectorField field);

uint8_t* GenerateVorticityHeatmap(VectorField field);

typedef enum {
    NORMALIZE,
    NON_NORMALIZE
} IntegratorNormalization;

typedef enum {
    INTEGRATOR_EULER,
    INTEGRATOR_4RK
} IntegratorType;

void ConvertPointsToNDC(vec2* _points, int _count, float _width, float _height);

void SampleField(VectorField* _field, float _x, float _y, vec2* _out);

void GetNormalizedFieldSample(VectorField* _field, float _x, float _y, vec2* _out);

vec2* GenerateFieldlineEuler(VectorField* _field, vec2 _seed, float _stepSize, int _maxSteps, int* _outCount, IntegratorNormalization _normalization);

vec2* GenerateFieldlineRK4(VectorField* _field, vec2 _seed, float _stepSize, int _maxSteps, int* _outCount, IntegratorNormalization _normalization);

vec2* GenerateFullFieldLine(VectorField* _field, vec2 _seed, float _stepSize, int _maxSteps, int* _outCount, IntegratorType _integratorType, IntegratorNormalization _normalization);