
#include "VkManager.h"
#include "VkBuffer.h"

void SetupBuffer(VkDevice _device, void* _data, VkBuffer* _buffer, VkDeviceMemory* _bufferMem, VkDeviceSize _size, VkBufferUsageFlags _usage)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Create staging buffer (CPU visible)
    CreateBuffer(_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    // Copy CPU data into staging buffer
    void* mapped;
    vkMapMemory(_device, stagingBufferMemory, 0, _size, 0, &mapped);
    memcpy(mapped, _data, (size_t)_size);
    vkUnmapMemory(_device, stagingBufferMemory);

    // Create final GPU buffer
    CreateBuffer(_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | _usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _buffer, _bufferMem );

    // Copy staging -> GPU
    CopyBuffer(stagingBuffer, *_buffer, _size);

    CleanupBuffer(stagingBuffer, stagingBufferMemory);
}

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

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties);

    if (vkAllocateMemory(gVkContext.mDevice, &allocInfo, NULL, _bufferMemory) != VK_SUCCESS) {
        LOG_ERROR("failed to allocate buffer memory");
    }

    vkBindBufferMemory(gVkContext.mDevice, *_buffer, *_bufferMemory, 0);
};

void CopyBuffer(VkBuffer _srcBuffer, VkBuffer _dstBuffer, VkDeviceSize _size){

    // make a temporary command buffer, since trasnfer of memory is done through the command chain, just like drawing.
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = gVkContext.mCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = {0};
    vkAllocateCommandBuffers(gVkContext.mDevice, &allocInfo, &commandBuffer);

    // start recording
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // calling once and waiting for completion of command, good to signify this with this flag.
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {0};
    copyRegion.size = _size;
    vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    // now that the command is recorded we can submit it
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(gVkContext.mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gVkContext.mGraphicsQueue);

    vkFreeCommandBuffers(gVkContext.mDevice, gVkContext.mCommandPool, 1, &commandBuffer);
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