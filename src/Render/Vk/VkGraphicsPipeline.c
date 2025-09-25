
#include "VKManager.h"
#include "VkGraphicsPipeline.h"

#include "string.h"

void SetupGraphicsPipeline(VkDevice _device){
   
    LOG_INFO("Setup VkGraphicsPipeline");
    
    size_t vertexCodeSize = 0;
    size_t fragmentCodeSize = 0;

    uint32_t* vertexSource = ReadShaderFile("shaders/vert.spv", "rb", &vertexCodeSize);
    uint32_t* fragSource = ReadShaderFile("shaders/frag.spv", "rb", &fragmentCodeSize);

    VkShaderModule vertexShaderModule = SetupShaderModule(_device, vertexSource, vertexCodeSize);
    VkShaderModule fragmentShaderModule = SetupShaderModule(_device, fragSource, fragmentCodeSize);

    VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[2] = {
        PopulatePipelineVertexShaderStageCreateInfo(vertexShaderModule),
        PopulatePipelineFragmentShaderStageCreateInfo(fragmentShaderModule)
    };

    vkDestroyShaderModule(_device, vertexShaderModule, NULL);
    vkDestroyShaderModule(_device, fragmentShaderModule, NULL);
    free(vertexSource);
    vertexSource = NULL;
    free(fragSource);
    fragSource = NULL;
    return;
};

uint32_t* ReadShaderFile(const char* _filePath, const char* _type, size_t* _codeSize){

    long filesize = GetFileSize(_filePath, _type);
    uint32_t* buffer = (uint32_t*)malloc(filesize);

    if(filesize <= 0){
        LOG_ERROR("File size 0 or smaller, failed alloc");
        return NULL;
    }
    if(!buffer){
        LOG_ERROR("Buffer memory allocation faild");
        return NULL;
    }

    FILE* fptr = fopen(_filePath, _type);
    size_t bytesRed = 0;
    if(fptr){

        bytesRed = fread(buffer, 1, filesize, fptr); // read into buffer, amount of size and from the file ptr.
        fclose(fptr);
    } else {
        LOG_ERROR("Faild to open file: %s", _filePath);
        return 0;
    }   

    if(bytesRed != filesize){
        LOG_ERROR("Faild to read complete shader file: %s", _filePath);
    }

    if(_codeSize){
        *_codeSize = filesize;
    }

    return buffer; 
};

long GetFileSize(const char* _filePath, const char* _type){

    FILE* fptr = fopen(_filePath, _type);
    long fileSize = 0;
    if(fptr){

        fseek(fptr, 0L, SEEK_END); // set file ptr to end of file
        fileSize = ftell(fptr); // retrives the position of file ptr, representing size
        fclose(fptr);
    } else {
        LOG_ERROR("Faild to open file: %s", _filePath);
        return 0;
    }   

    return fileSize;
};

VkShaderModule SetupShaderModule(VkDevice _device, uint32_t* _code, size_t _codeSize){

    VkShaderModule shaderModule = {0};

    VkShaderModuleCreateInfo createInfo = {0};

    PopulateShaderModule(&createInfo, _code, _codeSize);
    if (vkCreateShaderModule(_device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        LOG_ERROR("Faild to create shader module");
        return VK_NULL_HANDLE;
    }

    return shaderModule;
};

void PopulateShaderModule(VkShaderModuleCreateInfo* _createInfo, uint32_t* _code, size_t _codeSize){

    _createInfo->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    _createInfo->codeSize = _codeSize;
    _createInfo->pCode = _code;
};

VkPipelineShaderStageCreateInfo PopulatePipelineVertexShaderStageCreateInfo(VkShaderModule _vertexShaderModule){
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
    // first step is telling vk what pipline stage the shader is going to be used in. 
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    // next is the module containg the code, and the entry point of the code, being "main", set as the name.
    vertShaderStageInfo.module = _vertexShaderModule;
    vertShaderStageInfo.pName = "main";
    // the specializationInfo can specify values for shader constatnts before runtime. 
    //Meaning it can optimize better, left at null for now
    vertShaderStageInfo.pSpecializationInfo = NULL; 
    return vertShaderStageInfo;
};

VkPipelineShaderStageCreateInfo PopulatePipelineFragmentShaderStageCreateInfo(VkShaderModule _fragmentShaderModule){

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
    // same info for fragment shader as vertex shader
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = _fragmentShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.pSpecializationInfo = NULL; 
    return fragShaderStageInfo;
};

VkPipelineDynamicStateCreateInfo GetPiplineDynamicStateCreateInfo(){
        
    // These are the states we wish to define as dynamic
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    // This will cause these values to be ignored when initalizing, 
    // so that they can be configured during runtime. 
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    return dynamicState;
};

VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo(){

};

void CleanupGraphicsPipeline(){

    LOG_INFO("Cleanup VkGraphicsPipeline");

};