#include "ComponentSystems.h"
#include "Logger.h"
#include "VKManager.h"
#include "VkBuffer.h"
#include "HDF5Reader.h"
#include "UtilMath.h"

#include "stdlib.h"

/* --- MESH --- */
MeshComponent* gMeshComponentSystem = {0};
int gNumMeshComponents = 0;
TransformComponent* gTransformComponentSystem = {0};
int gNumTransformComponents = 0;

void InitMeshSystem(){

    gMeshComponentSystem = malloc(sizeof(MeshComponent) * gNumMeshComponents);
};

void AddMeshComponent(MeshComponent _mesh){

    // TODO:
    // verry inneficient, update to allocate more room and track actual vs max size.
    // make a sparse set to manage indexing vs components. 
    // create a general function for adding components that works for all types

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
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };
        numVertices = sizeof(planeVertices) / sizeof(Vertex);
        newMesh.mVertexCount = numVertices;
        newMesh.mVertices = malloc(sizeof(Vertex) * numVertices);
        memcpy(newMesh.mVertices, planeVertices, sizeof(Vertex) * numVertices);

        Index planeIndices[] = {
            0, 2, 1, 2, 0, 3
        };
        numIndices = sizeof(planeIndices) / sizeof(Index);
        newMesh.mIndexCount = numIndices;
        newMesh.mIndices = malloc(sizeof(Index) * numIndices);
        memcpy(newMesh.mIndices, planeIndices, sizeof(Index) * numIndices);
        SetupBuffer(gVkContext.mDevice, newMesh.mVertices, &newMesh.mVertexBuffer, &newMesh.mVertexBufferMemory, sizeof(Vertex) * newMesh.mVertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        SetupBuffer(gVkContext.mDevice, newMesh.mIndices, &newMesh.mIndexBuffer, &newMesh.mIndexBufferMemory, sizeof(Index) * newMesh.mIndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        break;
    case MESHTYPE_PLANE_WINDOW:
      
         Vertex planeWindowVertices[] = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };
        numVertices = sizeof(planeWindowVertices) / sizeof(Vertex);
        newMesh.mVertexCount = numVertices;
        newMesh.mVertices = malloc(sizeof(Vertex) * numVertices);
        memcpy(newMesh.mVertices, planeWindowVertices, sizeof(Vertex) * numVertices);

        Index planeWindowIndices[] = {
            0, 2, 1, 2, 0, 3
        };
        numIndices = sizeof(planeWindowIndices) / sizeof(Index);
        newMesh.mIndexCount = numIndices;
        newMesh.mIndices = malloc(sizeof(Index) * numIndices);
        memcpy(newMesh.mIndices, planeWindowIndices, sizeof(Index) * numIndices);
        SetupBuffer(gVkContext.mDevice, newMesh.mVertices, &newMesh.mVertexBuffer, &newMesh.mVertexBufferMemory, sizeof(Vertex) * newMesh.mVertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        SetupBuffer(gVkContext.mDevice, newMesh.mIndices, &newMesh.mIndexBuffer, &newMesh.mIndexBufferMemory, sizeof(Index) * newMesh.mIndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        break;
    case MESHTYPE_CUBE:
    //     Vertex cubeVertices[] = {
    //         // Front (+Z)
    //         {{-0.5f, -0.5f,  0.5f}, {1, 0, 0}},
    //         {{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}},
    //         {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}},
    //         {{-0.5f,  0.5f,  0.5f}, {1, 0, 0}},

    //         // Back (-Z)
    //         {{ 0.5f, -0.5f, -0.5f}, {0, 1, 0}},
    //         {{-0.5f, -0.5f, -0.5f}, {0, 1, 0}},
    //         {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}},
    //         {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}},

    //         // Left (-X)
    //         {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}},
    //         {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}},
    //         {{-0.5f,  0.5f,  0.5f}, {0, 0, 1}},
    //         {{-0.5f,  0.5f, -0.5f}, {0, 0, 1}},

    //         // Right (+X)
    //         {{ 0.5f, -0.5f,  0.5f}, {1, 1, 0}},
    //         {{ 0.5f, -0.5f, -0.5f}, {1, 1, 0}},
    //         {{ 0.5f,  0.5f, -0.5f}, {1, 1, 0}},
    //         {{ 0.5f,  0.5f,  0.5f}, {1, 1, 0}},

    //         // Top (+Y)
    //         {{-0.5f,  0.5f,  0.5f}, {0, 1, 1}},
    //         {{ 0.5f,  0.5f,  0.5f}, {0, 1, 1}},
    //         {{ 0.5f,  0.5f, -0.5f}, {0, 1, 1}},
    //         {{-0.5f,  0.5f, -0.5f}, {0, 1, 1}},

    //         // Bottom (-Y)
    //         {{-0.5f, -0.5f, -0.5f}, {1, 0, 1}},
    //         {{ 0.5f, -0.5f, -0.5f}, {1, 0, 1}},
    //         {{ 0.5f, -0.5f,  0.5f}, {1, 0, 1}},
    //         {{-0.5f, -0.5f,  0.5f}, {1, 0, 1}},
    //     };

    //     numVertices = sizeof(cubeVertices) / sizeof(Vertex);
    //     newMesh.mVertexCount = numVertices;
    //     newMesh.mVertices = malloc(sizeof(Vertex) * numVertices);
    //     memcpy(newMesh.mVertices, cubeVertices, sizeof(Vertex) * numVertices);

    //     Index cubeIndices[] = {
    //          0, 1, 2, 2, 3, 0,        // Front
    //          4, 5, 6, 6, 7, 4,        // Back
    //          8, 9,10,10,11, 8,        // Left
    //         12,13,14,14,15,12,        // Right
    //         16,17,18,18,19,16,        // Top
    //         20,21,22,22,23,20         // Bottom
    //     };

    //     numIndices = sizeof(cubeIndices) / sizeof(Index);
    //     newMesh.mIndexCount = numIndices;
    //     newMesh.mIndices = malloc(sizeof(Index) * numIndices);
    //     memcpy(newMesh.mIndices, cubeIndices, sizeof(Index) * numIndices);
    //     SetupBuffer(gVkContext.mDevice, newMesh.mVertices, &newMesh.mVertexBuffer, &newMesh.mVertexBufferMemory, sizeof(Vertex) * newMesh.mVertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    //     SetupBuffer(gVkContext.mDevice, newMesh.mIndices, &newMesh.mIndexBuffer, &newMesh.mIndexBufferMemory, sizeof(Index) * newMesh.mIndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        break;
    case MESHTYPE_PYRAMID:
        
        break;
    case MESHTYPE_SPHERE:
        
        break;
    default:
        LOG_WARN("MeshType Does not exist");
        break;
    }

    return newMesh; 
};

// MeshComponent GenerateVectorFieldMeshComponent(int _id, VectorField _vectorField){

//     size_t width = _vectorField.mWidth;
//     size_t height = _vectorField.mHeight;
//     size_t total = width * height;
//     size_t numVertices = total * 2;
//     size_t numIndices = total * 2;

//     float vectorLength = 0.01f;

//     Vertex* vertices = malloc(sizeof(Vertex) * numVertices);
//     Index* indices = malloc(sizeof(Index) * numIndices);

//     int numCurrentVertices = 0;
//     int numCurrentIndices = 0;

//     int itx = 0;
//     int ity = 0;
//     float stepSizex = 2.0f / width;
//     float stepSizey = 2.0f / height;
//     for(size_t i = 0; i < total; i++){

//         float posX = -1.0f + stepSizex * itx;
//         float posY = -1.0f + stepSizey * ity;
//         vec2 startPosition = {posX, posY};
//         itx++;
//         if(itx >= _vectorField.mWidth){
//             itx = 0;
//             ity ++;
//         }

//         vec2 direction;
//         glm_vec2_copy(_vectorField.mVectorField[i], direction);
//         glm_vec2_normalize(direction);

//         // scale direction
//         vec2 scaledDir;
//         glm_vec2_scale(direction, vectorLength, scaledDir);

//         // end = start + scaledDir
//         vec2 endPosition;
//         glm_vec2_add(startPosition, scaledDir, endPosition);
        
//         Vertex startPos = {0};
//         glm_vec2_copy(startPosition, startPos.mPosition);
//         glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, startPos.mColor);

//         Vertex endPos = {0};
//         glm_vec2_copy(endPosition, endPos.mPosition);
//         glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, endPos.mColor);

//         vertices[numCurrentVertices] = startPos;
//         indices[numCurrentIndices] = numCurrentVertices;
//         numCurrentIndices++;
//         numCurrentVertices++;

//         vertices[numCurrentVertices] = endPos;
//         indices[numCurrentIndices] = numCurrentVertices;
//         numCurrentIndices++;
//         numCurrentVertices++;

//         // ADD INDICES AND ARROW TIPS AS WELL
//         // DONT FORGET TO CHANGE TO WIRE STRIP MODE OR WHATEVER VK
//     }   


//     MeshComponent newMesh = {0};
//     newMesh.mID = _id;
//     newMesh.mVertexCount = numVertices;
//     newMesh.mVertices = malloc(sizeof(Vertex) * numVertices);
//     memcpy(newMesh.mVertices, vertices, sizeof(Vertex) * numVertices);

//     newMesh.mIndexCount = numIndices;
//     newMesh.mIndices = malloc(sizeof(Index) * numIndices);
//     memcpy(newMesh.mIndices, indices, sizeof(Index) * numIndices);
    
//     SetupBuffer(gVkContext.mDevice, newMesh.mVertices, &newMesh.mVertexBuffer, &newMesh.mVertexBufferMemory, sizeof(Vertex) * newMesh.mVertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
//     SetupBuffer(gVkContext.mDevice, newMesh.mIndices, &newMesh.mIndexBuffer, &newMesh.mIndexBufferMemory, sizeof(Index) * newMesh.mIndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

//     return newMesh;
// };

MeshComponent GenerateVectorFieldMeshComponent(int _id, VectorField _vectorField, float _vectorLength, float _resolutionScale, int _showMagnitude){

    float resolutionScale = _resolutionScale;
    CLAMP(resolutionScale, 0.001f, 1.f);
    float vectorLength = _vectorLength;
    CLAMP(vectorLength, 0.001f, 1.f);
    float tipSize = vectorLength * 0.3f;
    CLAMP(tipSize, 0.001f, 1.f);

    size_t width = _vectorField.mWidth;
    size_t height = _vectorField.mHeight;
    size_t total = width * height;

    float stepSizex = 2.0f / width;
    float stepSizey = 2.0f / height;

    size_t blockSize = (size_t)(1.0f / resolutionScale);
    blockSize = blockSize < 1 ? 1 : blockSize;
 
    size_t maxBlocksX = (width  + blockSize - 1) / blockSize;
    size_t maxBlocksY = (height + blockSize - 1) / blockSize;
    size_t maxSamples = maxBlocksX * maxBlocksY;

    size_t numVertices = maxSamples * 6;
    size_t numIndices = maxSamples * 6;

    Vertex* vertices = malloc(sizeof(Vertex) * numVertices);
    Index* indices = malloc(sizeof(Index) * numIndices);

    int numCurrentVertices = 0;
    int numCurrentIndices = 0;
    size_t v = 0;
    size_t i = 0;

    for (size_t by = 0; by < height; by += blockSize) {
        for (size_t bx = 0; bx < width; bx += blockSize) {

            // ---- AVERAGE ----
            vec2 avg = {0.0f, 0.0f};
            size_t count = 0;

            for (size_t y = by; y < by + blockSize && y < height; y++) {
                for (size_t x = bx; x < bx + blockSize && x < width; x++) {
                    size_t idx = y * width + x;
                    avg[0] += _vectorField.mVectorField[idx][0];
                    avg[1] += _vectorField.mVectorField[idx][1];
                    count++;
                }
            }

            if (count == 0) continue;

            avg[0] /= (float)count;
            avg[1] /= (float)count;

            float magnitude = glm_vec2_norm(avg);
            if (magnitude < 1e-6f) continue;

            glm_vec2_normalize(avg);

            // ---- POSITION ----
            size_t actualBlockWidth  = (bx + blockSize > width)  ? (width  - bx) : blockSize;
            size_t actualBlockHeight = (by + blockSize > height) ? (height - by) : blockSize;

            float centerX = (float)bx + (float)actualBlockWidth  * 0.5f;
            float centerY = (float)by + (float)actualBlockHeight * 0.5f;

            float posX = -1.0f + stepSizex * centerY; // X = centerY
            float posY = -1.0f + stepSizey * centerX; // Y = 1 - centerX (flip vertically)

            vec2 startPosition = {posX, posY};

            // ---- MAIN LINE ----
            vec2 scaledDir;
            if(_showMagnitude)
                glm_vec2_scale(avg, vectorLength * magnitude, scaledDir);
            else 
                glm_vec2_scale(avg, vectorLength, scaledDir);

            vec2 endPosition;
            glm_vec2_add(startPosition, scaledDir, endPosition);

            // ---- PERPENDICULAR ----
            vec2 perp = {-avg[1], avg[0]}; // 90° rotation

            // tip base (slightly back from end)
            vec2 back;
            glm_vec2_scale(avg, -tipSize, back);

            vec2 tipBase;
            glm_vec2_add(endPosition, back, tipBase);

            // left tip
            vec2 leftOffset;
            glm_vec2_scale(perp, tipSize * 0.5f, leftOffset);

            vec2 leftTip;
            glm_vec2_add(tipBase, leftOffset, leftTip);

            // right tip
            vec2 rightOffset;
            glm_vec2_scale(perp, -tipSize * 0.5f, rightOffset);

            vec2 rightTip;
            glm_vec2_add(tipBase, rightOffset, rightTip);

            // ---- CREATE VERTICES ----
            Vertex s = {0}, e = {0}, l = {0}, r = {0};

            glm_vec2_copy(startPosition, s.mPosition);
            glm_vec2_copy(endPosition,   e.mPosition);
            glm_vec2_copy(leftTip,       l.mPosition);
            glm_vec2_copy(rightTip,      r.mPosition);

            glm_vec3_copy((vec3){1,1,1}, s.mColor);
            glm_vec3_copy((vec3){1,1,1}, e.mColor);
            glm_vec3_copy((vec3){1,1,1}, l.mColor);
            glm_vec3_copy((vec3){1,1,1}, r.mColor);

            // main line
            vertices[v] = s; indices[i++] = v++;
            vertices[v] = e; indices[i++] = v++;

            // left tip line
            vertices[v] = e; indices[i++] = v++;
            vertices[v] = l; indices[i++] = v++;

            // right tip line
            vertices[v] = e; indices[i++] = v++;
            vertices[v] = r; indices[i++] = v++;
        }
    }   

    MeshComponent newMesh = {0};
    newMesh.mID = _id;
    newMesh.mVertexCount = numVertices;
    newMesh.mVertices = malloc(sizeof(Vertex) * numVertices);
    memcpy(newMesh.mVertices, vertices, sizeof(Vertex) * numVertices);

    newMesh.mIndexCount = numIndices;
    newMesh.mIndices = malloc(sizeof(Index) * numIndices);
    memcpy(newMesh.mIndices, indices, sizeof(Index) * numIndices);
    
    SetupBuffer(gVkContext.mDevice, newMesh.mVertices, &newMesh.mVertexBuffer, &newMesh.mVertexBufferMemory, sizeof(Vertex) * newMesh.mVertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    SetupBuffer(gVkContext.mDevice, newMesh.mIndices, &newMesh.mIndexBuffer, &newMesh.mIndexBufferMemory, sizeof(Index) * newMesh.mIndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    return newMesh;
};

void CleanupMesh(){

    vkDeviceWaitIdle(gVkContext.mDevice);
    for (int i = 0; i < gNumMeshComponents; i++) {
        free(gMeshComponentSystem[i].mVertices);
        free(gMeshComponentSystem[i].mIndices);
        
        CleanupBuffer(gMeshComponentSystem[i].mVertexBuffer, gMeshComponentSystem[i].mVertexBufferMemory);
        CleanupBuffer(gMeshComponentSystem[i].mIndexBuffer, gMeshComponentSystem[i].mIndexBufferMemory);
        
        if(gMeshComponentSystem[i].mTexture){
            CleanupTexture(gMeshComponentSystem[i].mTexture);
            free(gMeshComponentSystem[i].mTexture);
        }
    }
    
    free(gMeshComponentSystem);
};
/* --- MESH --- */

/* --- TRANSFORM --- */
void InitTransformSystem(){

    gTransformComponentSystem = malloc(sizeof(TransformComponent) * gNumTransformComponents);
};

void AddTransformComponent(TransformComponent _transformComp){
        // TODO:
    // verry inneficient, update to allocate more room and track actual vs max size.
    // make a sparse set to manage indexing vs components. 
    // create a general function for adding components that works for all types

    // Allocate temp memory and make temporary array.
    TransformComponent* tempTransSys = malloc(sizeof(TransformComponent) * gNumTransformComponents);
    if(!tempTransSys)
        LOG_ERROR("Faild to allocate memory for temporary TransformComponentSystem");
    
    // copy actual into temp
    for(int i = 0; i < gNumTransformComponents; i++)
        tempTransSys[i] = gTransformComponentSystem[i];
    
    // Itterate transform system and re allocate mesh array
    free(gTransformComponentSystem);
    gNumTransformComponents += 1;
    gTransformComponentSystem = malloc(sizeof(TransformComponent) * gNumTransformComponents);
    if(!gTransformComponentSystem)
        LOG_ERROR("Faild to allocate memory for TransformComponentSystem");

    // repopulate transform array
    for(int i = 0; i < gNumTransformComponents -1; i++){
        gTransformComponentSystem[i] = tempTransSys[i];
    }

    // Add the new transform and dealocate temp
    gTransformComponentSystem[gNumTransformComponents - 1] = _transformComp;
    free(tempTransSys);
};

TransformComponent CreateTransformComponent(int _id){

    TransformComponent transformComp = {0};
    transformComp.mID = _id;
    glm_vec3_one(transformComp.mScale);
    return transformComp;
};

void SetPosition(int _id, vec3 _pos){
    if((_id >= gNumTransformComponents) || (_id < 0)){
        LOG_WARN("SetPosition ID out of bounds.");
        return;
    }
        
    memcpy(gTransformComponentSystem[_id].mPosition, _pos, sizeof(vec3));
};

void SetRotation(int _id, vec3 _rot){
    if((_id >= gNumTransformComponents) || (_id < 0)){
        LOG_WARN("SetRotation ID out of bounds.");
        return;
    }
        
    memcpy(gTransformComponentSystem[_id].mRotation, _rot, sizeof(vec3));
};

void SetScale(int _id, vec3 _scale){
    if((_id >= gNumTransformComponents) || (_id < 0)){
        LOG_WARN("SetScale ID out of bounds.");
        return;
    }
        
    memcpy(gTransformComponentSystem[_id].mScale, _scale, sizeof(vec3));
};

float* GetPosition(int _id){
     if((_id >= gNumTransformComponents) || (_id < 0)){
        LOG_WARN("GetPosition ID out of bounds.");
        return NULL;
    }
        
    return gTransformComponentSystem[_id].mPosition;
};

float* GetRotation(int _id){
    if((_id >= gNumTransformComponents) || (_id < 0)){
        LOG_WARN("GetRotation ID out of bounds.");
        return NULL;
    }
        
    return gTransformComponentSystem[_id].mRotation;
};

float* GetScale(int _id){
     if((_id >= gNumTransformComponents) || (_id < 0)){
        LOG_WARN("GetScale ID out of bounds.");
        return NULL;
    }
        
    return gTransformComponentSystem[_id].mScale;
};


void CleanupTransform(){
    free(gTransformComponentSystem);
};

/* --- TRANSFORM --- */