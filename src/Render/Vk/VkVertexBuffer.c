
#include "VkVertexBuffer.h"
#include "Vertex.h"
#include "VKManager.h"  

void SetupVertexBuffer(VkDevice _device){
    LOG_INFO("Setup VkVertexBuffer and VertexBufferMemory");

    // Vertex Buffer ---
    VkBuffer vertexBuffer;
    VkBufferCreateInfo bufferInfo = {0};
    PopulateVertexBufferCreateInfo(&bufferInfo, NUM_VERTICES);

    if (vkCreateBuffer(_device, &bufferInfo, NULL, &vertexBuffer) != VK_SUCCESS) {
        LOG_ERROR("Faild creating vk Vertex buffer");
        return;
    }
    gVkContext.mVertexBuffer = vertexBuffer;

    // Vertex Memory ---
    VkDeviceMemory vertexBufferMemory;
    VkMemoryAllocateInfo allocInfo = {0};
    VkMemoryRequirements memRequirements = {0};
    vkGetBufferMemoryRequirements(_device, vertexBuffer, &memRequirements);
    PopulateVertexMemoryAllocateInfo(&allocInfo, memRequirements);

    if (vkAllocateMemory(_device, &allocInfo, NULL, &vertexBufferMemory) != VK_SUCCESS) {
        LOG_ERROR("Failed to allocate vertex buffer memory");
        return;
    }
    gVkContext.mVertexBufferMemory = vertexBufferMemory;

    // Buffer and memory success we can then bind em.
    vkBindBufferMemory(_device, vertexBuffer, vertexBufferMemory, 0);

    // Fill vertex buffer with vertex data
    void* data;
    vkMapMemory(_device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, &testVertices, (size_t) bufferInfo.size);
    vkUnmapMemory(_device, vertexBufferMemory);

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