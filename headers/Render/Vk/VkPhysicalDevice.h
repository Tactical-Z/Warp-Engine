#pragma once 

#include "vulkan.h"

// Setts up the best suited phyical device for this application.
// @param _inst is the vk instance.
// @param _surface is the vk surface for device seletion.
// @return Is the best suited physical device for this application.
VkPhysicalDevice SetupPhysicalDevice(VkInstance _inst, VkSurfaceKHR _surface);

// Gets all availible physical devices for this application.
// @param _vki Is the vk instance referenced during device creation.
// @param _numDevices is filled through reference for the number of devices availible.
// @return Returns an array of physical devices availible for this application. 
VkPhysicalDevice* GetAvailibleDevices(VkInstance _vki, int* _numDevices);

// Gets the best suited device from the input array of availble devices.
// @param _vki is the vk instance.
// @param _surface is the vk surface for device selection.
// @param _availableDevices is an array of valid devices to pick from.
// @param _numAvailibleDevices are the number of devices availble in the array passed previously.
// @return is the best suited physical device for this application.
VkPhysicalDevice GetBestSuitedPhysicalDevice(VkInstance _vki, VkSurfaceKHR _surface, VkPhysicalDevice* _availableDevices, int _numAvailibleDevices);

// Checks if device is suitable for this surface and application
// @param _device is the device to check
// @param _surface is the vk surface
// @return is 1 for suitable, 0 otherwise
int isDeviceSuitable(VkPhysicalDevice _device, VkSurfaceKHR _surface);

// Checks if device extension is suported for this physical devices
// @param _physicalDevice is the physical device to check against
// @return is 1 if the reqired extension for this physical device is availble, 0 otherwise
int isDeviceExtensionSupportAvailable(VkPhysicalDevice _physicalDevice);

// Rates the input physical device based on how well it is suited for this application.
// @param _physicalDevice is the device to rate. 
// @return Is the value of the physical device as an int.
int RateDevice(VkPhysicalDevice _physicalDevice);

// Finds the queue families and updates through reference, returning for success or failure.
// @param _physicalDevice is the physical device to find a queue family for.
// @param _surface is the vk surface to reference.
// @param _familyQueue is the family queue filled through reference.
// @return 1 if a queue is found for the device, 0 otherwise.
int FindQueueFamilies(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, QueueFamilyIndices* _familyQueue);

// Cleans up the Physical Device
void CleanupPhysicalDevice();
