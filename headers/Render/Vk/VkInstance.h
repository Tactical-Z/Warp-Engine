#pragma once

#include "vulkan.h"

VkInstance CreateInstance();
void PopulateVkInstanceCreateInfo(struct VkInstanceCreateInfo* _createInfo, struct VkApplicationInfo* _appInfo, struct VkDebugUtilsMessengerCreateInfoEXT* _initDebugCreateInfo);
void CleanupInstance();