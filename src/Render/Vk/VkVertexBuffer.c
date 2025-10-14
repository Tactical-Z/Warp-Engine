
#include "VkVertexBuffer.h"
#include "Vertex.h"

VkPhysicalDevice SetupVertexBuffer();

void PopulateVertexBufferCreateInfo(VkFramebufferCreateInfo* _createInfo, int _imageviewIndex);

void CleanupVertexBuffer();