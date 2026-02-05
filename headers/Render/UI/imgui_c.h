#pragma once
#include "vulkan.h"

// This makes the header C compatible for the compiler.
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
void ImGuiC_Init(void* _glfwWindow,
                 void* _vkInstance,
                 void* _vkPhysicalDevice,
                 void* _vkDevice,
                 uint32_t _VkQueueFamily,
                 void* _vkQueue,
                 void* _vkDescriptorPool,
                 uint32_t _swapchainImageCount,
                 void* _vkRenderPass);
void ImGuiC_LoadFonts(void* _vkDevice,
                      void* _vkCommandPool,
                      void* _vkQueue);
void ImGuiC_BeginFrame();
void ImGuiC_EndFrame(void *vkCommandBuffer);
void ImGuiC_Shutdown(void* _vkDevice, 
                     void* _vkuiDescriptorPool);

// vulcan spesific commands
VkCommandBuffer UiC_BeginSingleTimeCommands(VkDevice _device, VkCommandPool _commandPool);
void UiC_EndSingleTimeCommands(VkDevice _device, VkCommandPool _commandPool, VkQueue _queue, VkCommandBuffer _commandBuffer);

void ImGuiC_BeginWindow(const char* name);
void ImGuiC_EndWindow();
void ImGuiC_Text(const char* text);
int  ImGuiC_Button(const char* label);

#ifdef __cplusplus
}
#endif