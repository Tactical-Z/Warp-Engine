
#include "VkVertexBuffer.h"
#include "Vertex.h"
#include "VKManager.h"  

void SetupVertexBuffer(VkDevice _device){
    LOG_INFO("Setup VkVertexBuffer and VertexBufferMemory");

    VkDeviceSize bufferSize = sizeof(testVertices);

    // We are now uing these two flags to create a source and destination of the vertex buffer
    // VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Buffer can be used as source in a memory transfer operation.
    // VK_BUFFER_USAGE_TRANSFER_DST_BIT: Buffer can be used as destination in a memory transfer operation.
    VkBuffer stagingBuffer = {0};
    VkDeviceMemory stagingBufferMemory = {0};
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    
    void* data;
    vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, testVertices, (size_t) bufferSize);
    vkUnmapMemory(_device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gVkContext.mVertexBuffer, &gVkContext.mVertexBufferMemory);
    CopyBuffer(stagingBuffer,gVkContext.mVertexBuffer, bufferSize);
    
    vkDestroyBuffer(_device, stagingBuffer, NULL);
    vkFreeMemory(_device, stagingBufferMemory, NULL);
};

void PopulateVertexBufferCreateInfo(VkBufferCreateInfo* _createInfo, int _numVertices){
    
    _createInfo->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    _createInfo->size = sizeof(Vertex) * _numVertices;
    // usage defines for what perpouse the data in buffer is going to be used. 
    _createInfo->usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    _createInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

};

void PopulateVertexMemoryAllocateInfo(VkMemoryAllocateInfo* _createInfo, VkMemoryRequirements _memRequirements){


    _createInfo->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    _createInfo->allocationSize = _memRequirements.size;
    _createInfo->memoryTypeIndex = FindMemoryType(_memRequirements.memoryTypeBits, 
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

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

void CleanupVertexBuffer(){

    LOG_INFO("Cleanup VkVertexBuffer and VkVertexBufferMemory");
    vkDestroyBuffer(gVkContext.mDevice, gVkContext.mVertexBuffer, NULL);
    vkFreeMemory(gVkContext.mDevice, gVkContext.mVertexBufferMemory, NULL);
};