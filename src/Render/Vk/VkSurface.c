
#include "VKManager.h"
#include "VkSurface.h"

VkSurfaceKHR SetupSurface(GLFWwindow* _window, VkInstance _inst){
    
    LOG_INFO("Setup VkSurface");
    VkSurfaceKHR surface = {0};

    if(glfwCreateWindowSurface(_inst, _window, NULL, &surface) != VK_SUCCESS){
        LOG_ERROR("Faild to create window surface");
    }

    return surface;
};

void CleanupSurface(){

    LOG_INFO("Cleanup VkSurface");
    vkDestroySurfaceKHR(gVkContext.mInstance, gVkContext.mSurface, NULL);
};
