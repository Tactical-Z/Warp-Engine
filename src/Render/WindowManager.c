
#include "WindowManager.h"
#include "VKManager.h"

GLFWwindow* InitWindow(int _width, int _height, const char* _name){
    return InitGLFW(_width, _height, _name);
}

GLFWwindow* InitGLFW(int _width, int _height, const char* _name){
 
    if(!glfwInit()){
        LOG_ERROR("GLFW faild to initalize");
        return NULL;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    return InitGLFWWindow(_width, _height, _name);
}

GLFWwindow* InitGLFWWindow(int _width, int _height, const char* _name){
    // Create window
    GLFWwindow* window = glfwCreateWindow(_width, _height, _name, NULL, NULL);

    // Set callbacks for window
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    return window;
}

void Run(GLFWwindow* _glfwWindow){

    int frames = 0;
    while (!glfwWindowShouldClose(_glfwWindow))
    {
        glfwSwapBuffers(_glfwWindow);
        glfwPollEvents();

        DrawFrame(_glfwWindow);

        frames++;
        if (frames > 500000){
            LOG_INFO("Window still ticking...");
            frames = 0;
        }
    }
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    gVkContext.mFramebufferResized = 1;
}

int CleanupWindow(GLFWwindow* _glfwWindow){
    glfwDestroyWindow(_glfwWindow);
    glfwTerminate();
    
    return 0;
}