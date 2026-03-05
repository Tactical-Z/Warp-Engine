#pragma once

#include "glfw3.h"

void SceneBegin();

void SceneUpdate(float _dt);

void SceneRender(GLFWwindow* _glfwWindow);

void SceneCleanup();