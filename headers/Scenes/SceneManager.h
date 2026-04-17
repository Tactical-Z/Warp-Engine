#pragma once

#include "glfw3.h"
#include "UtilMath.h"
#include "HDF5Reader.h"

void SceneBegin();

void ExmapleInit();

void GenerateFieldLinesFromVectorField(VectorField* _vecField, float _stepSize, int _maxSteps, IntegratorType _integratorType, IntegratorNormalization _normalization, int _numSeeds, SeedGenerator _seedGenerator, int _renderFieldLines, int _renderSeeds);

void SceneUpdate(float _dt);

void SceneRender(GLFWwindow* _glfwWindow);

void SceneCleanup();