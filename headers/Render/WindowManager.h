#pragma once

#include "Logger.h"
#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

GLFWwindow* InitWindow(int _width, int _height, const char* _name);
GLFWwindow* InitGLFW(int _width, int _height, const char* _name);
GLFWwindow* InitGLFWWindow(int _width, int _height, const char* _name);

void Run(GLFWwindow* _glfwWindow);

static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

int CleanupWindow(GLFWwindow* _glfwWindow);

