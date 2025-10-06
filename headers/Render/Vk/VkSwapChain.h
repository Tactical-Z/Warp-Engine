#pragma once

#include "vulkan.h"
#include "glfw3.h"

// swap chain support details struct containing iomportant formats and presentation modes and arrays.
typedef struct {
    VkSurfaceCapabilitiesKHR mCapabilities;
    uint32_t mFormatCount;
    VkSurfaceFormatKHR* mFormats;
    uint32_t mPresentModeCount;
    VkPresentModeKHR* mPresentModes;
}SwapChainSupportDetails;

// Setts up the swapchain for visualisation .
// @param _window is the GLFW window referenced for swapchain detials.
// @param _device is the logical device that will use the swapchain.
// @param _physicalDevice is the physical device spesifing some details for creation.
// @param _surface is the vk surface the swapchain will refence.
// @return The VKSwpachain Struct. 
VkSwapchainKHR SetupSwapChain(GLFWwindow* _window, VkDevice _device, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

int RecreateSwapchain(GLFWwindow* _window);

// Querys the swapchain for spesific details and returns them
// @param _physicalDevice the VK Physical device.
// @param _surface is the vk surface.
// @return SwapChainSupportDetails struct containing presentation and format modes.
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);

// Populates the swapchain create info.
// @param _creatInfo is passed htrough reference and filled.
// @param _physicalDevice is used to fill create info
// @param _surface is the vk surface
// @param _swapChainSupportDetails are additional details for create info.
// @param _window is a GLFW window reference. 
void PopulateSwapChain(VkSwapchainCreateInfoKHR* _createInfo, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, SwapChainSupportDetails _swapChainSupport , GLFWwindow* _window);

// Checker function that returns wether or not a swapchain is adaquate.
// @param _swapChain is the swapchain in question.
// @return 1 for adaquate, 0 otherwise.
int isSwapChainAdaquate(SwapChainSupportDetails* _swapchain);

// Decides waht sufrace format the swapchain shall use
// @param _availibleFormats is a pointer to an array of all availible formats
// @param _numModes is the number of formats in the array.
// @return The decided surface format. 
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR* _availibleFormats, uint32_t _numFormats); 

// Decides waht presentation mode the swapchain shall use
// @param _availibleFormats is a pointer to an array of all availible modes
// @param _numModes is the number of modes in the array.
// @return The decided presentation mode.
VkPresentModeKHR ChooseSwapPresentationMode(VkPresentModeKHR* _availibleModes, uint32_t _numModes);

// Takes in the capabilites of the suraface and chooses an apropriate extent for the swapchain.
// @param _capabilities are the surface capabilites for this application.
// @param _window is the GLFW window
VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR _capabilities, GLFWwindow* _window);

// Cleans up the Vk SwapChain
void CleanupSwapChain();