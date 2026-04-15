
#include "WindowManager.h"
#include "VkManager.h"
#include "ComponentSystems.h"
#include "SceneManager.h"
#include "Windows.h"

double gDeltaTime = 0.0;
static LARGE_INTEGER gFrequency;
static LARGE_INTEGER gLastTime;

GLFWwindow* InitWindow(int _width, int _height, const char* _name){
    return InitGLFW(_width, _height, _name);
}

GLFWwindow* InitGLFW(int _width, int _height, const char* _name){
 
    if(!glfwInit()){
        LOG_ERROR("GLFW faild to initalize");
        return NULL;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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
    InitTimer();
    InitComponentSystems();
    SceneBegin();
    while (!glfwWindowShouldClose(_glfwWindow))
    {
        glfwSwapBuffers(_glfwWindow);
        glfwPollEvents();

        double dt = UpdateDeltatime();
        SceneUpdate(dt);
        SceneRender(_glfwWindow);
    }
    SceneCleanup();
}

void InitTimer(){
    QueryPerformanceFrequency(&gFrequency);
    QueryPerformanceCounter(&gLastTime);
}

double UpdateDeltatime(){
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    gDeltaTime = (double)(currentTime.QuadPart - gLastTime.QuadPart) /
                 (double)gFrequency.QuadPart;

    gLastTime = currentTime;
    return gDeltaTime;
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    gVkContext.mFramebufferResized = 1;
}

int CleanupWindow(GLFWwindow* _glfwWindow){
    glfwDestroyWindow(_glfwWindow);
    glfwTerminate();
    
    return 0;
}