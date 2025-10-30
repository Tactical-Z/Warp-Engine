
#include "VkIndexBuffer.h"
#include "Vertex.h"
#include "VKManager.h"  

void SetupIndexBuffer(VkDevice _device){
    LOG_INFO("Setup VkIndexBuffer and IndexBufferMemory");

    VkDeviceSize bufferSize = sizeof(testIndices);

    VkBuffer stagingBuffer = {0};
    VkDeviceMemory stagingBufferMemory = {0};
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, testIndices, (size_t) bufferSize);
    vkUnmapMemory(_device, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gVkContext.mIndexBuffer, &gVkContext.mIndexBufferMemory);

    CopyBuffer(stagingBuffer, gVkContext.mIndexBuffer, bufferSize);

    vkDestroyBuffer(_device, stagingBuffer, NULL);
    vkFreeMemory(_device, stagingBufferMemory, NULL);
};


void CleanupIndexBuffer(){

    LOG_INFO("Cleanup VkIndexBuffer and VkIndexBufferMemory");
    vkDestroyBuffer(gVkContext.mDevice, gVkContext.mIndexBuffer, NULL);
    vkFreeMemory(gVkContext.mDevice, gVkContext.mIndexBufferMemory, NULL);
};