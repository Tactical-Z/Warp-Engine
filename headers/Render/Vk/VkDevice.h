#pragma once

#include "vulkan.h"

// Sets up the Vk logic Device from the vk instance, surface and physical device.
// The Logical Device is used to interface with the physical device through pipelines and queues.
// @return The Vk Logic Device
VkDevice SetupDevice(VkInstance _inst, VkSurfaceKHR _surface, VkPhysicalDevice _physicalDevice, QueueFamilyIndices* _familyIndices);

// Gets an array to the create infoes for all device queues. Is responsible for describing the number of queues we want for a single queue family.
// @param _family is passed as a pointer and filled by this function
// @return An array of create info created by the input queue family indices
VkDeviceQueueCreateInfo* GetVkDeviceQueueCreateInfo(QueueFamilyIndices* _family);

// Gets the physical device features for this application like geometry shaders.
// no fetures yet. 
VkPhysicalDeviceFeatures GetVkPhysicalDeviceFeatures();

// Populates the Vk Device create ingo using the array of queue familys and device features
// @param _createInfo Is the create info passed through reference this function fills. 
// @param _physicalDevice Is the vk physical device for error handeling.
// @param _deviceQueueCreateInfo is a pointer to an array of create infoes for each device queue.
// @param _numQueues is the number of queues to use.
// @param _physicalDeviceFeatures is the features the logic device should include.
void PopulateVkDevice(VkDeviceCreateInfo* _createInfo, VkPhysicalDevice _physicalDevice,  VkDeviceQueueCreateInfo* _deviceQueueCreateInfo, int _numQueues, VkPhysicalDeviceFeatures* _physicalDeviceFeatures);

// Cleans up the VkInstance
void CleanupDevice();