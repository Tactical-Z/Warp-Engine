#include "UIManager.h"

#include "imgui_c.h"

void InitUI()
{
    ImGuiC_Init();
}

void DrawUI()
{
    ImGuiC_BeginFrame();

    // UI to draw goes here:
    StarterWindow();

    ImGuiC_EndFrame();
}

void StarterWindow()
{
    ImGuiC_BeginWindow("Hello ImGui");
}

int ShutdownUI()
{
    ImGuiC_Shutdown();
    return 0;
}
