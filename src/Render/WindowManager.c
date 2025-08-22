
#include "WindowManager.h"

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
    return glfwCreateWindow(_width, _height, _name, NULL, NULL);
}

void Run(GLFWwindow* _glfwWindow){

    int frames = 0;
    while (!glfwWindowShouldClose(_glfwWindow))
    {
        glfwSwapBuffers(_glfwWindow);
        glfwPollEvents();

        frames++;
        if (frames > 500000){
            LOG_INFO("Window still ticking...");
            frames = 0;
        }
    }
}

int CleanupWindow(GLFWwindow* _glfwWindow){
    glfwDestroyWindow(_glfwWindow);
    glfwTerminate();
    
    return 0;
}