#include "UIManager.h"
#include "VKManager.h"

#include "imgui_c.h"

void InitUI(void* _glfwWindow)
{
    ImGuiC_Init(_glfwWindow, 
                &gVkContext.mInstance,
                gVkContext.mPhysicalDevice,
                gVkContext.mDevice,
                gVkContext.mDeviceSupportDetails.mQueueFamily.mGraphicsFamily,
                gVkContext.mGraphicsQueue,
                gVkContext.mUiDescriptorPool,
                gVkSwapChainHandles.mNumImages,
                gVkContext.mRenderPass);
    
    ImGuiC_LoadFonts(gVkContext.mDevice, 
                     gVkContext.mCommandPool, 
                     gVkContext.mGraphicsQueue);
}

void DrawUI()
{
    ImGuiC_BeginFrame();

    // UI to draw goes here:
    //StarterWindow();

    ImGuiC_EndFrame(gVkContext.mCommandBuffers[gCurrentFrame]);
}

void StarterWindow()
{
    ImGuiC_BeginWindow("Hello ImGui");

}

int ShutdownUI()
{
    ImGuiC_Shutdown(gVkContext.mDevice, gVkContext.mUiDescriptorPool);
    return 0;
}
