
#include "VKManager.h"
#include "VkGraphicsPipeline.h"
#include "Vertex.h"
#include "string.h"

PipelineCreateInfoContext vkPipelineCreateInfoContext = {0};

VkPipeline SetupGraphicsPipeline(VkDevice _device){
   
    LOG_INFO("Setup VkGraphicsPipeline");
    
    // shaders
    size_t vertexCodeSize = 0;
    size_t fragmentCodeSize = 0;

    uint32_t* vertexSource = ReadShaderFile("shaders/vert.spv", "rb", &vertexCodeSize);
    uint32_t* fragSource = ReadShaderFile("shaders/frag.spv", "rb", &fragmentCodeSize);

    VkShaderModule vertexShaderModule = SetupShaderModule(_device, vertexSource, vertexCodeSize);
    VkShaderModule fragmentShaderModule = SetupShaderModule(_device, fragSource, fragmentCodeSize);

    VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[NUM_SHADER_STAGES] = {
        GetPipelineVertexShaderStageCreateInfo(vertexShaderModule),
        GetPipelineFragmentShaderStageCreateInfo(fragmentShaderModule)
    };

    // pipeline
    SetupPipelineLayout(_device);
    VkPipeline graphicsPipeline = {0};
    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    PopulateGraphicsPipeline(&pipelineInfo, NUM_SHADER_STAGES,shaderStagesCreateInfo);

    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS) {
        LOG_ERROR("Faild VkPipeline (graphics) creation");
    }

    vkDestroyShaderModule(_device, vertexShaderModule, NULL);
    vkDestroyShaderModule(_device, fragmentShaderModule, NULL);
    free(vertexSource);
    vertexSource = NULL;
    free(fragSource);
    fragSource = NULL;
    return graphicsPipeline;
};

void PopulateGraphicsPipeline(VkGraphicsPipelineCreateInfo* _createInfo, uint32_t _numShadersStages, VkPipelineShaderStageCreateInfo* _shaderStageCreateInfo){

    PopulatePipelineVertexInputStateCreateInfo(&vkPipelineCreateInfoContext.mVertexInputInfo);
    PopulatePipelineInputAssemblyStateCreateInfo(&vkPipelineCreateInfoContext.mInputAssembly);
    PopulateViewPort(&vkPipelineCreateInfoContext.mViewport);
    PopulateScissor(&vkPipelineCreateInfoContext.mScissor);
    PopulatePipelineViewportStateCreateInfo(&vkPipelineCreateInfoContext.mViewportState, &vkPipelineCreateInfoContext.mViewport, &vkPipelineCreateInfoContext.mScissor);
    PopulatePipelineRasterizationStateCreateInfo(&vkPipelineCreateInfoContext.mRasterizer);
    PopulatePipelineMultisampleStateCreateInfo(&vkPipelineCreateInfoContext.mMultisampling);
    PopulatePipelineDepthStencilStateCreateInfo(&vkPipelineCreateInfoContext.mDepthStencil);
    PopulatePipelineColorBlendAttachmentState(&vkPipelineCreateInfoContext.mColorBlendAttachment);
    PopulatePipelineColorBlendStateCreateInfo(&vkPipelineCreateInfoContext.mColorBlending, &vkPipelineCreateInfoContext.mColorBlendAttachment);
    PopulatePipelineDynamicStateCreateInfo(&vkPipelineCreateInfoContext.mDynamicState);

    // type
    _createInfo->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // Shader stages
    _createInfo->stageCount = _numShadersStages;
    _createInfo->pStages = _shaderStageCreateInfo;
    // Fixed function stages
    _createInfo->pVertexInputState = &vkPipelineCreateInfoContext.mVertexInputInfo;
    _createInfo->pInputAssemblyState = &vkPipelineCreateInfoContext.mInputAssembly;
    _createInfo->pViewportState = &vkPipelineCreateInfoContext.mViewportState;
    _createInfo->pRasterizationState = &vkPipelineCreateInfoContext.mRasterizer;
    _createInfo->pMultisampleState = &vkPipelineCreateInfoContext.mMultisampling;
    _createInfo->pDepthStencilState = NULL; //vkPipelineCreateInfoContext.mDepthStencil; // Optional
    _createInfo->pColorBlendState = &vkPipelineCreateInfoContext.mColorBlending;
    _createInfo->pDynamicState = &vkPipelineCreateInfoContext.mDynamicState;
    // Lauout
    _createInfo->layout = gVkContext.mPipelineLayout;
    // Render pass
    _createInfo->renderPass = gVkContext.mRenderPass;
    _createInfo->subpass = 0;
    // incase of making more pipelines this spesifies copy details.
    _createInfo->basePipelineHandle = VK_NULL_HANDLE; // Optional
    _createInfo->basePipelineIndex = -1; // Optional
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

VkPipelineShaderStageCreateInfo GetPipelineVertexShaderStageCreateInfo(VkShaderModule _vertexShaderModule){
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

VkPipelineShaderStageCreateInfo GetPipelineFragmentShaderStageCreateInfo(VkShaderModule _fragmentShaderModule){

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
    // same info for fragment shader as vertex shader
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = _fragmentShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.pSpecializationInfo = NULL; 
    return fragShaderStageInfo;
};

// ----------------------

void PopulatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo* _createInfo){
    vkPipelineCreateInfoContext.mBindingDescription = GetBindingDescription();
    vkPipelineCreateInfoContext.mAttributeDescriptions = GetAttributeDescriptions(NUM_VERTEX_ATTRIBUTES);
    
    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    _createInfo->vertexBindingDescriptionCount = 1;
    _createInfo->pVertexBindingDescriptions = &vkPipelineCreateInfoContext.mBindingDescription;
    _createInfo->vertexAttributeDescriptionCount = NUM_VERTEX_ATTRIBUTES;
    _createInfo->pVertexAttributeDescriptions = vkPipelineCreateInfoContext.mAttributeDescriptions;
};

VkVertexInputBindingDescription GetBindingDescription(){
    
    VkVertexInputBindingDescription bindingDescription = {0};
    // One binding since it is all packed togeather in one array
    // specifies the index of the binding in the array of bindings
    bindingDescription.binding = 0;
    // num bytes between vertices
    bindingDescription.stride = sizeof(Vertex);
    // Input rate     
        // VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
        // VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
};

VkVertexInputAttributeDescription* GetAttributeDescriptions(){

    VkVertexInputAttributeDescription* attributeDescriptions = malloc(sizeof(VkVertexInputAttributeDescription) * NUM_VERTEX_ATTRIBUTES);
    memset(attributeDescriptions, 0, NUM_VERTEX_ATTRIBUTES);

    // binding tells vulkan from which binding the per vertex data comes
    attributeDescriptions[0].binding = 0;
    // location references the location in the vertex shader
    attributeDescriptions[0].location = 0;
    // Describes the type of data for the attribte, formated to colors:
        // float: VK_FORMAT_R32_SFLOAT
        // vec2: VK_FORMAT_R32G32_SFLOAT
        // vec3: VK_FORMAT_R32G32B32_SFLOAT
        // vec4: VK_FORMAT_R32G32B32A32_SFLOAT
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    // num bytes to tead for this attribute
    attributeDescriptions[0].offset = offsetof(Vertex, mPosition);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, mColor);

    return attributeDescriptions;
};

void PopulatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo* _createInfo){
  
    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _createInfo->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    _createInfo->primitiveRestartEnable = VK_FALSE;

};

void PopulatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo* _createInfo, VkViewport* _viewport, VkRect2D* _scissor){

    // only use this create info for static construction of a viewport!
    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _createInfo->viewportCount = 1;
    _createInfo->pViewports = NULL; // _viewport; disabled since using dynamic viewport
    _createInfo->scissorCount = 1;
    _createInfo->pScissors = NULL; // _scissor; disabled since using dynamic scissor
};

void PopulateViewPort(VkViewport* _createInfo){

    // Min and max depth specify the range of depth values to use for the framebuffer
    // must be within [0.f, 1.f]
    _createInfo->x = 0.0f;
    _createInfo->y = 0.0f;
    _createInfo->width = (float)gVkSwapChainHandles.mSwapChainExtent.width;
    _createInfo->height = (float)gVkSwapChainHandles.mSwapChainExtent.height;
    _createInfo->minDepth = 0.0f;
    _createInfo->maxDepth = 1.0f;
};

void PopulateScissor(VkRect2D* _createInfo){

    // make same size as framebuffer images so that nothing is cut away
    VkOffset2D offset = {0, 0};
    _createInfo->offset = offset;
    _createInfo->extent = gVkSwapChainHandles.mSwapChainExtent;
};

void PopulatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo* _createInfo){

    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // if depthclamp is enabled it does not cull geometry behind or infront of far planes. Just clamps them to the planes.
    _createInfo->depthClampEnable = VK_FALSE;
    // if discard is enabled then geometru never passes through the rasterization stage. 
    _createInfo->rasterizerDiscardEnable = VK_FALSE;
    // Polygon mode determins how fragments are gnereated for goeometry:
        // VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
        // VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
        // VK_POLYGON_MODE_POINT: polygon vertices are drawn as points
    _createInfo->polygonMode = VK_POLYGON_MODE_FILL;
    _createInfo->lineWidth = 1.0f;
    // cullmode defines what faces to cull 
    _createInfo->cullMode = VK_CULL_MODE_BACK_BIT;
    _createInfo->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    // depth bias is used for altering depth values, good for shadows.
    _createInfo->depthBiasEnable = VK_FALSE;
    _createInfo->depthBiasConstantFactor = 0.0f; // Optional
    _createInfo->depthBiasClamp = 0.0f; // Optional
    _createInfo->depthBiasSlopeFactor = 0.0f; // Optional
};

void PopulatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo* _createInfo){

    // Disabled for now
    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _createInfo->sampleShadingEnable = VK_FALSE;
    _createInfo->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _createInfo->minSampleShading = 1.0f; // Optional
    _createInfo->pSampleMask = NULL; // Optional
    _createInfo->alphaToCoverageEnable = VK_FALSE; // Optional
    _createInfo->alphaToOneEnable = VK_FALSE; // Optional
};

void PopulatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo* _createInfo){


};

void PopulatePipelineColorBlendAttachmentState(VkPipelineColorBlendAttachmentState* _createInfo){

    _createInfo->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    _createInfo->blendEnable = VK_FALSE;
    _createInfo->srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    _createInfo->dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    _createInfo->colorBlendOp = VK_BLEND_OP_ADD; // Optional
    _createInfo->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    _createInfo->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    _createInfo->alphaBlendOp = VK_BLEND_OP_ADD; // Optional
};

void PopulatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo* _createInfo, VkPipelineColorBlendAttachmentState* _attachmentState){

    // Scond color blending option, collect into one total struct:
    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _createInfo->logicOpEnable = VK_FALSE;
    _createInfo->logicOp = VK_LOGIC_OP_COPY; // Optional
    _createInfo->attachmentCount = 1;
    _createInfo->pAttachments = _attachmentState;
    _createInfo->blendConstants[0] = 0.0f; // Optional
    _createInfo->blendConstants[1] = 0.0f; // Optional
    _createInfo->blendConstants[2] = 0.0f; // Optional
    _createInfo->blendConstants[3] = 0.0f; // Optional
};

void PopulatePipelineDynamicStateCreateInfo(VkPipelineDynamicStateCreateInfo* _createInfo){
        
    // This will cause these values to be ignored when initalizing, 
    // so that they can be configured during runtime. 
    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    _createInfo->dynamicStateCount = NUM_DYNAMIC_PIPELINESTATES;
    _createInfo->pDynamicStates = sDynamicStates;
};

void SetupPipelineLayout(VkDevice _device){

    VkPipelineLayout pipelineLayout = {0};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    PopulatePipelineLayoutCreateInfo(&pipelineLayoutInfo);

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        LOG_ERROR("Pipeline layout creation faild");
        return;
    }

    gVkContext.mPipelineLayout = pipelineLayout;
};

void PopulatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo* _createInfo){

    VkPushConstantRange pushConstantRange = {0};
    PopulatePipelinePushConstantRangeCreateInfo(&pushConstantRange);

    _createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    _createInfo->setLayoutCount = 1;
    _createInfo->pSetLayouts = &gVkContext.mDescriptorSetLayout;
    _createInfo->pushConstantRangeCount = 1;
    _createInfo->pPushConstantRanges = &pushConstantRange;
};

void PopulatePipelinePushConstantRangeCreateInfo(VkPushConstantRange* _createInfo){

    _createInfo->stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    _createInfo->offset = 0;
    _createInfo->size = sizeof(mat4);
};

void CleanupGraphicsPipeline(){

    LOG_INFO("Cleanup VkGraphicsPipeline");
    free(vkPipelineCreateInfoContext.mAttributeDescriptions);
    vkPipelineCreateInfoContext.mAttributeDescriptions = NULL;
    vkDestroyPipeline(gVkContext.mDevice, gVkContext.mGraphicsPipeline, NULL);
    vkDestroyPipelineLayout(gVkContext.mDevice, gVkContext.mPipelineLayout, NULL);
};