
#include "VKManager.h"
#include "VkGraphicsPipeline.h"

#include "string.h"

void SetupGraphicsPipeline(VkDevice _device){
   
    LOG_INFO("Setup VkGraphicsPipeline");
    
    const char* vertexSource = ReadShaderFile("shaders/vert.spv", "rb");
    const char* fragSource = ReadShaderFile("shaders/frag.spv", "rb");

    VkShaderModule vertexShaderModule = SetupShaderModule(_device, vertexSource);
    VkShaderModule fragmentShaderModule = SetupShaderModule(_device, fragSource);

    return;
};

const char* ReadShaderFile(const char* _filePath, const char* _type){

    long filesize = GetFileSize(_filePath, _type);
    char* buffer = malloc(sizeof(const char) * filesize);

    FILE* fptr = fopen(_filePath, _type);
    if(fptr){

        fgets(buffer, filesize, fptr); // read into buffer, amount of size and from the file ptr.
        fclose(fptr);
    } else {
        LOG_ERROR("Faild to open file: %s", _filePath);
        return 0;
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

VkShaderModule SetupShaderModule(VkDevice _device, const char* _code){

    VkShaderModule shaderModule = {0};

    VkShaderModuleCreateInfo createInfo = {0};
    

    PopulateShaderModule(&createInfo, _code);
    if (vkCreateShaderModule(_device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        LOG_ERROR("Faild to create shader module");
        return VK_NULL_HANDLE;
    }

    return shaderModule;
};

void PopulateShaderModule(VkShaderModuleCreateInfo* _createInfo, const char* _code){

    size_t codeSize = strlen(_code);

    _createInfo->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    _createInfo->codeSize = codeSize;
    _createInfo->pCode = (const uint32_t*)_code;
};

void CleanupGraphicsPipeline(){

    LOG_INFO("Cleanup VkGraphicsPipeline");

};