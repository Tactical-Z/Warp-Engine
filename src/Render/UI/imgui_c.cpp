#include "imgui_c.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "glfw3.h"

#include "Logger.h"

extern "C" {

// All the c++ code can be written here for ImGui, but called through C wrapper. 
void ImGuiC_Init(void* _glfwWindow,
                 void* _vkInstance,
                 void* _vkPhysicalDevice,
                 void* _vkDevice,
                 uint32_t _VkQueueFamily,
                 void* _vkQueue,
                 void* _vkDescriptorPool,
                 uint32_t _swapchainImageCount,
                 void* _vkRenderPass)
{
    GLFWwindow* window = (GLFWwindow*)_glfwWindow;
    VkInstance instance = (VkInstance)_vkInstance;
    VkPhysicalDevice physical = (VkPhysicalDevice)_vkPhysicalDevice;
    VkDevice device = (VkDevice)_vkDevice;
    VkQueue queue = (VkQueue)_vkQueue;
    VkDescriptorPool pool = (VkDescriptorPool)_vkDescriptorPool;
    VkRenderPass rp = (VkRenderPass)_vkRenderPass;

    // Basic Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Vulcan init
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physical;
    init_info.Device = device;
    init_info.QueueFamily = _VkQueueFamily;
    init_info.Queue = queue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = pool;
    init_info.MinImageCount = _swapchainImageCount;
    init_info.ImageCount = _swapchainImageCount;
    ImGui_ImplVulkan_Init(&init_info, rp);
}

void ImGuiC_LoadFonts(void* _vkDevice, void* _vkCommandPool, void* _vkQueue){
    VkDevice device = (VkDevice)_vkDevice;
    VkCommandPool commandPool = (VkCommandPool)_vkCommandPool;
    VkQueue queue = (VkQueue)_vkQueue;

    // Upload the font texture
    VkCommandBuffer cmd = UiC_BeginSingleTimeCommands(device, commandPool);
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    UiC_EndSingleTimeCommands(device, commandPool, queue, cmd);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void ImGuiC_BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiC_EndFrame(void* vkCommandBuffer)
{
    VkCommandBuffer cmd = (VkCommandBuffer)vkCommandBuffer;

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void ImGuiC_Shutdown(void* _vkDevice, void* _vkuiDescriptorPool)
{
    VkDevice device = (VkDevice)_vkDevice;
    VkDescriptorPool pool = (VkDescriptorPool)_vkuiDescriptorPool;
    vkDeviceWaitIdle(device);

    ImGui_ImplVulkan_DestroyFontUploadObjects();
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(device, pool, nullptr);

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

VkCommandBuffer UiC_BeginSingleTimeCommands(VkDevice _device, VkCommandPool _commandPool){
    
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = {};
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
};

void UiC_EndSingleTimeCommands(VkDevice _device, VkCommandPool _commandPool, VkQueue _queue, VkCommandBuffer _commandBuffer){
    vkEndCommandBuffer(_commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;

    vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_queue);

    vkFreeCommandBuffers(_device, _commandPool, 1, &_commandBuffer);
};


void ImGuiC_BeginWindow(const char* name)
{
    
    if(ImGui::Begin(name)){
        
        if(ImGui::Button("Lessgoo")){
            LOG_DEBUG("LESSSGOOO");
        };

        ImGui::End();
    };

}

void ImGuiC_EndWindow()
{
    ImGui::End();
}

void ImGuiC_Text(const char* text)
{
    ImGui::TextUnformatted(text);
}

int ImGuiC_Button(const char* label)
{
    return ImGui::Button(label) ? 1 : 0;
}
} //extern "C"