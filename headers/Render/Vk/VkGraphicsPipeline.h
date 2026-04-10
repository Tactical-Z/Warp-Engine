#pragma once

#include "vulkan.h"

#define NUM_SHADER_STAGES 2

// These are the states we wish to define as dynamic
#define NUM_DYNAMIC_PIPELINESTATES 2
static VkDynamicState sDynamicStates[NUM_DYNAMIC_PIPELINESTATES] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

typedef struct {
    VkPipelineVertexInputStateCreateInfo mVertexInputInfo;
    VkVertexInputBindingDescription mBindingDescription;
    VkVertexInputAttributeDescription* mAttributeDescriptions;
    VkPipelineInputAssemblyStateCreateInfo mInputAssembly;
    VkViewport mViewport;
    VkRect2D mScissor;
    VkPipelineViewportStateCreateInfo mViewportState;
    VkPipelineRasterizationStateCreateInfo mRasterizer;
    VkPipelineMultisampleStateCreateInfo mMultisampling;
    VkPipelineDepthStencilStateCreateInfo mDepthStencil;
    VkPipelineColorBlendStateCreateInfo mColorBlending;
    VkPipelineColorBlendAttachmentState mColorBlendAttachment;
    VkPipelineDynamicStateCreateInfo mDynamicState;
} PipelineCreateInfoContext;

// Setts up the graphics pipeline for vk rendering.
VkPipeline SetupGraphicsPipeline(VkDevice _device);

// Populates the graphics pipeline create info. 
void PopulateGraphicsPipeline(VkGraphicsPipelineCreateInfo* _createInfo, uint32_t _numShadersStages, VkPipelineShaderStageCreateInfo* _shaderStageCreateInfo);

// helper file functions TODO: move somewhere
uint32_t* ReadShaderFile(const char* _filePath, const char* _type, size_t* _codeSize);
long GetFileSizeSeb(const char* _filePath, const char* _type);

// Creats the shader modules by importing the binary spv(SPIR-V) shader file data. Shader modules are just a thin wrapper around the bytecode
// they can be local verriables since they linking and compilation makes the local veriables unnessasry. 
// @param _device is the vk device.
// @param _code is the bytecode of the SPIR-V as a uint32 array.
// @param _codeSize is the size of the array in bytes.
// @return Is the vk shader module. 
VkShaderModule SetupShaderModule(VkDevice _device, uint32_t* _code, size_t _codeSize);

// Populates the shader modules create info with the spv code and info. 
void PopulateShaderModule(VkShaderModuleCreateInfo* _createInfo, uint32_t* _code, size_t _codeSize);

// Assign the vertex shader modules to the pipeline using the pipeline shader stage create infor struct. 
VkPipelineShaderStageCreateInfo GetPipelineVertexShaderStageCreateInfo(VkShaderModule _vertexShaderModule);

// Assign the fragment shader modules to the pipeline using the pipeline shader stage create infor struct. 
VkPipelineShaderStageCreateInfo GetPipelineFragmentShaderStageCreateInfo(VkShaderModule _fragmentShaderModule);

// --------------------------

// Gets the vertex imput state create info for the vk graphics pipeline. 
// It describes the format of the vertex dat athat will be passed to the vertex shader.
// descibes in two ways, bindings (spacing between data) and attribute descriptions (types of attributes passed to the vertex shader)
void PopulatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo* _createInfo);

// Gets the binding description for vertexes
VkVertexInputBindingDescription GetBindingDescription();

// Gets the attribute descriptions for vertex attributes
VkVertexInputAttributeDescription* GetAttributeDescriptions();

// Decribes two things;
// 1, what kind of geometry will be drawn from the verices.
// 2, if primitive restatrt should be enabled
void PopulatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo* _createInfo);

// The viewport basically describes the region of the framebuffer that the output will be rendered to.
// almost alawys 0,0,width,heigh
void PopulateViewPort(VkViewport* _createInfo);

// Defines which region of pixles to show, any image outside of scissor will be discareded by rasterizer. 
void PopulateScissor(VkRect2D* _createInfo);

// Creates and returns the viewport create info with the viewport and scissor details, 
// only use this if making static viewport and not dynamic.
void PopulatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo* _createInfo, VkViewport* _viewport, VkRect2D* _scissor);

// Spesifies rasterization details for processing geometry after vertex shader, 
// can do things like depth testing, face culling, wirefreame and so on.
void PopulatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo* _createInfo);

// Configures multisampleing, one way to perform anti-aliasing. It works by combining the fragment
// shader results of multiple polygons that rasterize to the smae pixel. Requires enabaling a gpu feature.
void PopulatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo* _createInfo);

// Depth and stencil buffer configurations.
void PopulatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo* _createInfo);

// Color attahcment type
void PopulatePipelineColorBlendAttachmentState(VkPipelineColorBlendAttachmentState* _createInfo);

// After fragment shader has returned a color it needs to be combined with the color that is allready in the framebuffer.
// this tranformation is called color vlending and is done in two ways:
    // 1, Mix the old and new value to preduce new color.
    // 2, Combine the old and new vlaue vusing a bitwise operation.
    // we are using color blend attachment state since we only have one framebuffer.
void PopulatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo* _createInfo, VkPipelineColorBlendAttachmentState* _attachmentState);

// Since most of the pipeline is baked into the pipline state, we need to manualy define states that can be changed in run time.
void PopulatePipelineDynamicStateCreateInfo(VkPipelineDynamicStateCreateInfo* _createInfo);


// Sets the layout of the pipline, it is required to have, but can be used to manage uniform shader values.
void SetupPipelineLayout(VkDevice _device);

// Gets the create info for the pipeline layout 
void PopulatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo* _createInfo);

void PopulatePipelinePushConstantRangeCreateInfo(VkPushConstantRange* _createInfo);

// Cleans up the Vk Graphics pipeline
void CleanupGraphicsPipeline();