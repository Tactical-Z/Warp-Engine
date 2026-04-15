
#include "SceneManager.h"
#include "ComponentSystems.h"
#include "VKManager.h"
#include "VkDescriptorSetBuffer.h"
#include "Texture.h"
#include "FileSystem.h"
#include "HDF5Reader.h"
#include "UtilMath.h"

void SceneBegin(){

    VectorField vecField = Readhdf5File(AssetDir("VisData/isabel_2d.h5"), DATASET_ISABEL, "/Velocity/X-comp", "/Velocity/Y-comp");
    //VectorField vecField = Readhdf5File(AssetDir("VisData/metsim1_2d.h5"), DATASET_METSIM, "/Velocity/X-comp", "/Velocity/Y-comp");
  
    // Default leaf
    //AddMeshComponent(CreateMeshComponent(0, MESHTYPE_PLANE_WINDOW));
    //AddTransformComponent(CreateTransformComponent(0));
    //gMeshComponentSystem[0].mTexture = LoadTextureImage(AssetDir("TestLeaf.jpg"));
    //gMeshComponentSystem[0].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[0]);

    // Heat map
    //AddMeshComponent(CreateMeshComponent(0, MESHTYPE_PLANE_WINDOW));
    //gMeshComponentSystem[0].mTexture = CreateTexture(GenerateMagnitudeHeatmap(vecField),vecField.mWidth, vecField.mHeight);
    //gMeshComponentSystem[0].mTexture = CreateTexture(GenerateVorticityHeatmap(vecField),vecField.mWidth, vecField.mHeight);
    //gMeshComponentSystem[0].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[0]);
    
    // vector field
    AddMeshComponent(GenerateVectorFieldMeshComponent(0, vecField, 0.03f, 0.1, 0));
    AddTransformComponent(CreateTransformComponent(0));
    gMeshComponentSystem[0].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[0]);
    
    //
    //for (int y = 0; y < vecField.mHeight; y += 50) {
    //    for (int x = 0; x < vecField.mWidth; x += 50) {
        //
    //        size_t idx = y * vecField.mWidth + x;
        //
    //        LOG_DEBUG("(%d,%d): %f %f",
    //            x, y,
    //            vecField.mVectorField[idx][0],
    //            vecField.mVectorField[idx][1]);
    //    }
    //}
    //vec2 vecfieldAtLocation = {0};
    //SampleField(&vecField, 50, 50, &vecfieldAtLocation);
    //LOG_DEBUG("vector field x: %f  y:%f", vecfieldAtLocation[0],vecfieldAtLocation[1]);

    // test line
    // vec3 color = {1,0.5,0.2};
    // vec2 points[] = {
    //     {-1.0f, -1.0f},
    //     {-0.2f, -0.2f},
    //     {0.0f, 0.0f},
    //     {0.8f, 0.6f},
    //     {1.0f, 1.0f}
    // };
    // size_t count = sizeof(points) / sizeof(points[0]);
    // AddMeshComponent(CreateLineMeshFromArray(1, points, count, color));
    // AddTransformComponent(CreateTransformComponent(1));
    // gMeshComponentSystem[1].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[1]);

    // Field line 
    vec3 color = {1, 0, 0};
    vec2 seedPoint = {300,300};
    int totalCount = 0;
    vec2* fieldLinePoints = GenerateFullFieldLine(&vecField, seedPoint, 0.1, 5000, &totalCount, INTEGRATOR_EULER, NON_NORMALIZE);
    AddMeshComponent(CreateLineMeshFromArray(1, fieldLinePoints, totalCount, color));
    if(gMeshComponentSystem[1].mVertexCount > 0){
        AddTransformComponent(CreateTransformComponent(1));
        gMeshComponentSystem[1].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[1]);
    }
    free(fieldLinePoints);

    // Field line RK4
    vec3 colornew = {0, 0, 1.0};
    vec2 seedPointnew = {300,300};
    int totalCountnew = 0;
    vec2* fieldLinePointsOther = GenerateFullFieldLine(&vecField, seedPointnew, 0.1, 5000, &totalCountnew, INTEGRATOR_4RK, NON_NORMALIZE);
    AddMeshComponent(CreateLineMeshFromArray(2, fieldLinePointsOther, totalCountnew, colornew));
    if(gMeshComponentSystem[2].mVertexCount > 0){
        AddTransformComponent(CreateTransformComponent(2));
        gMeshComponentSystem[2].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[2]);
    }
    free(fieldLinePointsOther);

    free(vecField.mVectorField);

}

void SceneUpdate(float _dt){

    // Move object
    //vec3 pos;
    //glm_vec3_copy(GetPosition(0), pos);
    //pos[1] += _dt;
    //SetPosition(0, pos);
}

void SceneRender(GLFWwindow* _glfwWindow){
    DrawFrame(_glfwWindow);
}

void SceneCleanup(){

    CleanupMesh();
}