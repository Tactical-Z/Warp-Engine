
#include "VkVertexBuffer.h"
#include "Vertex.h"
#include "VKManager.h"  

VkBuffer SetupVertexBuffer(VkDevice _device){
    LOG_INFO("Setup VkVertexBuffer");

    VkBuffer vertexBuffer;
    VkBufferCreateInfo bufferInfo = {0};
    PopulateVertexBufferCreateInfo(&bufferInfo, 3);

    if (vkCreateBuffer(_device, &bufferInfo, NULL, &vertexBuffer) != VK_SUCCESS) {
        LOG_ERROR("Faild creating vk Vertex buffer");
        return VK_NULL_HANDLE;
    }

    return vertexBuffer;
};

void PopulateVertexBufferCreateInfo(VkBufferCreateInfo* _createInfo, int _numVertices){
    
    _createInfo->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    _createInfo->size = sizeof(Vertex) * _numVertices;
    // usage defines for what perpouse the data in buffer is going to be used. 
    _createInfo->usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    _createInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

};

void CleanupVertexBuffer(){

    LOG_INFO("Cleanup VkVertexBuffer");
    vkDestroyBuffer(gVkContext.mDevice, gVkContext.mVertexBuffer, NULL);
};