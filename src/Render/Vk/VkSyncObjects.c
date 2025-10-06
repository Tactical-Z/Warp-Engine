
#include "VKManager.h"
#include "VkSyncObjects.h"

int SetupSyncObjects(VkDevice _device){
    LOG_INFO("Setup Vk SyncObjects");

    uint32_t imageCount = gVkSwapChainHandles.mNumImages;

    gVkContext.mImageAvailableSemaphores = malloc(sizeof(VkSemaphore) * imageCount);
    memset(gVkContext.mImageAvailableSemaphores, 0, imageCount);

    gVkContext.mRenderFinishedSemaphores = malloc(sizeof(VkSemaphore) * imageCount);
    memset(gVkContext.mRenderFinishedSemaphores, 0, imageCount);

    gVkContext.mInFlightFences = malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);
    memset(gVkContext.mInFlightFences, 0, MAX_FRAMES_IN_FLIGHT);

    for(int i = 0; i < imageCount; i++){
        VkSemaphoreCreateInfo semaphoreCreateInfo = {0};
        VkFenceCreateInfo fenceCreateInfo = {0};
        PopulateSemaphoreCreateInfo(&semaphoreCreateInfo);
        PopulateFenCereateInfo(&fenceCreateInfo);

        if (vkCreateSemaphore(_device, &semaphoreCreateInfo, NULL, &gVkContext.mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_device, &semaphoreCreateInfo, NULL, &gVkContext.mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_device, &fenceCreateInfo, NULL, &gVkContext.mInFlightFences[i]) != VK_SUCCESS) {
            
            LOG_ERROR("Faild to create Vk SyncObjects");
            return 0;
        }
    }
   
    return 1;
};

void PopulateSemaphoreCreateInfo(VkSemaphoreCreateInfo* _createInfo){
    _createInfo->sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
};

void PopulateFenCereateInfo(VkFenceCreateInfo* _createInfo){
    _createInfo->sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    _createInfo->flags = VK_FENCE_CREATE_SIGNALED_BIT; // so wait for fence does not stop first frame
};

void CleanupSyncObjects(){

    LOG_INFO("Cleanup Vk SyncObjects");
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore(gVkContext.mDevice, gVkContext.mImageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(gVkContext.mDevice, gVkContext.mRenderFinishedSemaphores[i], NULL);
        vkDestroyFence(gVkContext.mDevice, gVkContext.mInFlightFences[i], NULL);
    }
   
    free(gVkContext.mImageAvailableSemaphores);
    gVkContext.mImageAvailableSemaphores = NULL;
    free(gVkContext.mRenderFinishedSemaphores);
    gVkContext.mRenderFinishedSemaphores = NULL;
    free(gVkContext.mInFlightFences);
    gVkContext.mInFlightFences = NULL;
};