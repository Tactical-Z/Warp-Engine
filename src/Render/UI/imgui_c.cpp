#include "imgui_c.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

// All the c++ code can be written here for ImGui, but called through C wrapper. 
void ImGuiC_Init(void)
{

}

void ImGuiC_BeginFrame(void)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiC_EndFrame(void)
{
    ImGui::Render();
}

void ImGuiC_Shutdown(void)
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiC_BeginWindow(const char* name)
{
    ImGui::Begin(name);
}

void ImGuiC_EndWindow(void)
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