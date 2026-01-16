#pragma once

// This makes the header C compatible for the compiler.
#ifdef __cplusplus
extern "C" {
#endif

void ImGuiC_Init(void);
void ImGuiC_BeginFrame(void);
void ImGuiC_EndFrame(void);
void ImGuiC_Shutdown(void);

void ImGuiC_BeginWindow(const char* name);
void ImGuiC_EndWindow(void);
void ImGuiC_Text(const char* text);
int  ImGuiC_Button(const char* label);

#ifdef __cplusplus
}
#endif