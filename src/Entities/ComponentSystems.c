#include "ComponentSystems.h"
#include "Logger.h"
#include "VKManager.h"
#include "VkBuffer.h"

#include "stdlib.h"

/* --- MESH --- */
MeshComponent* gMeshComponentSystem = {0};
int gNumMeshComponents = 0;

void InitMeshSystem(){

    gMeshComponentSystem = malloc(sizeof(MeshComponent) * gNumMeshComponents);
};

void AddMesh(MeshComponent _mesh){

    // TODO:
    // verry inneficient, update to allocate more room and track actual vs max size.
    // make a sparse set to manage indexing vs components. 

    // Allocate temp memory and make temporary array.
    MeshComponent* tempMeshSys = malloc(sizeof(MeshComponent) * gNumMeshComponents);
    if(!tempMeshSys)
        LOG_ERROR("Faild to allocate memory for temporary MeshComponentSystem");
    
    // copy actual into temp
    for(int i = 0; i < gNumMeshComponents; i++)
        tempMeshSys[i] = gMeshComponentSystem[i];
    
    // Itterate mesh system and re allocate mesh array
    free(gMeshComponentSystem);
    gNumMeshComponents += 1;
    gMeshComponentSystem = malloc(sizeof(MeshComponent) * gNumMeshComponents);
    if(!gMeshComponentSystem)
        LOG_ERROR("Faild to allocate memory for ComponentMeshSystem");

    // repopulate mesh array
    for(int i = 0; i < gNumMeshComponents -1; i++){
        gMeshComponentSystem[i] = tempMeshSys[i];
    }

    // Add the new Mesh and dealocate temp
    gMeshComponentSystem[gNumMeshComponents - 1] = _mesh;
    free(tempMeshSys);
};

MeshComponent CreateMeshComponent(int _id, MeshType _meshType){

    MeshComponent newMesh = {0};
    newMesh.mID = _id;
    int numVertices = 0;
    int numIndices = 0;

    switch (_meshType)
    {
    case MESHTYPE_PLANE:
      
        Vertex planeVertices[] = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };
        numVertices = sizeof(planeVertices) / sizeof(Vertex);
        newMesh.mVertexCount = numVertices;
        newMesh.mVertices = malloc(sizeof(Vertex) * numVertices);
        memcpy(newMesh.mVertices, planeVertices, sizeof(Vertex) * numVertices);

        Index planeIndices[] = {
            0, 1, 2, 2, 3, 0
        };
        numIndices = sizeof(planeIndices) / sizeof(Index);
        newMesh.mIndexCount = numIndices;
        newMesh.mIndices = malloc(sizeof(Index) * numIndices);
        memcpy(newMesh.mIndices, planeIndices, sizeof(Index) * numIndices);
        SetupBuffer(gVkContext.mDevice, newMesh.mVertices, &newMesh.mVertexBuffer, &newMesh.mVertexBufferMemory, sizeof(Vertex) * newMesh.mVertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        SetupBuffer(gVkContext.mDevice, newMesh.mIndices, &newMesh.mIndexBuffer, &newMesh.mIndexBufferMemory, sizeof(Index) * newMesh.mIndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        break;
    case MESHTYPE_CUBE:
        Vertex cubeVertices[] = {
            // Front (+Z)
            {{-0.5f, -0.5f,  0.5f}, {1, 0, 0}},
            {{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}},
            {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}},
            {{-0.5f,  0.5f,  0.5f}, {1, 0, 0}},

            // Back (-Z)
            {{ 0.5f, -0.5f, -0.5f}, {0, 1, 0}},
            {{-0.5f, -0.5f, -0.5f}, {0, 1, 0}},
            {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}},

            // Left (-X)
            {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}},
            {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}},
            {{-0.5f,  0.5f,  0.5f}, {0, 0, 1}},
            {{-0.5f,  0.5f, -0.5f}, {0, 0, 1}},

            // Right (+X)
            {{ 0.5f, -0.5f,  0.5f}, {1, 1, 0}},
            {{ 0.5f, -0.5f, -0.5f}, {1, 1, 0}},
            {{ 0.5f,  0.5f, -0.5f}, {1, 1, 0}},
            {{ 0.5f,  0.5f,  0.5f}, {1, 1, 0}},

            // Top (+Y)
            {{-0.5f,  0.5f,  0.5f}, {0, 1, 1}},
            {{ 0.5f,  0.5f,  0.5f}, {0, 1, 1}},
            {{ 0.5f,  0.5f, -0.5f}, {0, 1, 1}},
            {{-0.5f,  0.5f, -0.5f}, {0, 1, 1}},

            // Bottom (-Y)
            {{-0.5f, -0.5f, -0.5f}, {1, 0, 1}},
            {{ 0.5f, -0.5f, -0.5f}, {1, 0, 1}},
            {{ 0.5f, -0.5f,  0.5f}, {1, 0, 1}},
            {{-0.5f, -0.5f,  0.5f}, {1, 0, 1}},
        };

        numVertices = sizeof(cubeVertices) / sizeof(Vertex);
        newMesh.mVertexCount = numVertices;
        newMesh.mVertices = malloc(sizeof(Vertex) * numVertices);
        memcpy(newMesh.mVertices, cubeVertices, sizeof(Vertex) * numVertices);

        uint32_t cubeIndices[] = {
             0, 1, 2, 2, 3, 0,        // Front
             4, 5, 6, 6, 7, 4,        // Back
             8, 9,10,10,11, 8,        // Left
            12,13,14,14,15,12,        // Right
            16,17,18,18,19,16,        // Top
            20,21,22,22,23,20         // Bottom
        };

        numIndices = sizeof(cubeIndices) / sizeof(Index);
        newMesh.mIndexCount = numIndices;
        newMesh.mIndices = malloc(sizeof(Index) * numIndices);
        memcpy(newMesh.mIndices, cubeIndices, sizeof(Index) * numIndices);
        SetupBuffer(gVkContext.mDevice, newMesh.mVertices, &newMesh.mVertexBuffer, &newMesh.mVertexBufferMemory, sizeof(Vertex) * newMesh.mVertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        SetupBuffer(gVkContext.mDevice, newMesh.mIndices, &newMesh.mIndexBuffer, &newMesh.mIndexBufferMemory, sizeof(Index) * newMesh.mIndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        break;
    case MESHTYPE_PYRAMID:
        
        break;
    case MESHTYPE_SPHERE:
        
        break;
    default:
        LOG_WARN("MeshType Does not exist.");
        break;
    }

    return newMesh; 
};

void CleanupMesh(){

    vkDeviceWaitIdle(gVkContext.mDevice);
    for (int i = 0; i < gNumMeshComponents; i++) {
        free(gMeshComponentSystem[i].mVertices);
        free(gMeshComponentSystem[i].mIndices);
        
        CleanupBuffer(gMeshComponentSystem[i].mVertexBuffer, gMeshComponentSystem[i].mVertexBufferMemory);
        CleanupBuffer(gMeshComponentSystem[i].mIndexBuffer, gMeshComponentSystem[i].mIndexBufferMemory);
    }
    
    free(gMeshComponentSystem);
};
/* --- MESH --- */