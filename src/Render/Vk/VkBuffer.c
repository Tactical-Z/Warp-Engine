
#include "VkManager.h"
#include "VkBuffer.h"
#include "VkCommandBuffer.h"

void SetupBuffer(VkDevice _device, void* _data, VkBuffer* _buffer, VkDeviceMemory* _bufferMem, VkDeviceSize _size, VkBufferUsageFlags _usage)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Create staging buffer (CPU visible)
    CreateBuffer(_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    // Copy CPU data into staging buffer
    UploadToMemory(_device, stagingBufferMemory, _data,_size);

    // Create final GPU buffer
    CreateBuffer(_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | _usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _bufferMem );

    // Copy staging -> GPU
    CopyBuffer(stagingBuffer, *_buffer, _size);

    CleanupBuffer(stagingBuffer, stagingBufferMemory);
}

VkResult UploadToMemory(VkDevice _device, VkDeviceMemory _memory, const void* _srcData, VkDeviceSize _size)
{
    void* dst;
    VkResult result = vkMapMemory(_device, _memory, 0, _size, 0, &dst);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed Uploading Memory to buffer.");
        return result;
    }
    memcpy(dst, _srcData, (size_t)_size);
    vkUnmapMemory(_device, _memory);

    return VK_SUCCESS;
};

void CreateBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer* _buffer, VkDeviceMemory* _bufferMemory){
    
    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = _size;
    bufferInfo.usage = _usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(gVkContext.mDevice, &bufferInfo, NULL, _buffer) != VK_SUCCESS) {
        LOG_ERROR("failed to create buffer");
    }

    VkMemoryRequirements memRequirements = {0};
    vkGetBufferMemoryRequirements(gVkContext.mDevice, *_buffer, &memRequirements);
    AllocateMemory(gVkContext.mDevice, memRequirements,_properties,_bufferMemory);

    vkBindBufferMemory(gVkContext.mDevice, *_buffer, *_bufferMemory, 0);
};

void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size){

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(gVkContext.mDevice, gVkContext.mCommandPool);

    VkBufferCopy copyRegion = {0};
    copyRegion.size = _size;
    vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(gVkContext.mDevice, gVkContext.mCommandPool, gVkContext.mGraphicsQueue, commandBuffer);
};

void AllocateMemory(VkDevice _device, VkMemoryRequirements _memRequirements, VkMemoryPropertyFlags _properties, VkDeviceMemory* _bindMemory){

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = _memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(_memRequirements.memoryTypeBits, _properties);

    if (vkAllocateMemory(_device, &allocInfo, NULL, _bindMemory) != VK_SUCCESS) {
        LOG_ERROR("failed to allocate image memory");
        return;
    }
};

uint32_t FindMemoryType(uint32_t _typeFilter, VkMemoryPropertyFlags _properties)
{
    // First check what types of memory are availble from the physical device.
    // has two arrays, memory  types and memory heaps (like RAM/VRAM)
    VkPhysicalDeviceMemoryProperties memProperties = {0};
    vkGetPhysicalDeviceMemoryProperties(gVkContext.mPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (_typeFilter & (1 << i)  && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties) {
            return i;
        }
    }

    LOG_ERROR("Faild to find suitable memory type.");
    return -1;
};

void CleanupBuffer(VkBuffer _buffer, VkDeviceMemory _bufferMem){
        
    vkDestroyBuffer(gVkContext.mDevice, _buffer, NULL);
    vkFreeMemory(gVkContext.mDevice, _bufferMem, NULL);
}