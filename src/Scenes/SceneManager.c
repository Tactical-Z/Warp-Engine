
#include "SceneManager.h"
#include "ComponentSystems.h"
#include "VKManager.h"
#include "VkDescriptorSetBuffer.h"
#include "Texture.h"
#include "FileSystem.h"
#include "HDF5Reader.h"
#include "UtilMath.h"

void SceneBegin(){

    //vec2* vecField = Readhdf5File(AssetDir("VisData/isabel_2d.h5"), "/Velocity/X-comp", "/Velocity/Y-comp");
    vec2* vecField = Readhdf5File(AssetDir("VisData/metsim1_2d.h5"), "/Velocity/X-comp", "/Velocity/Y-comp");
    
    AddMeshComponent(CreateMeshComponent(0, MESHTYPE_PLANE_WINDOW));
    AddTransformComponent(CreateTransformComponent(0));
    //gMeshComponentSystem[0].mTexture = LoadTexture(AssetDir("TestLeaf.jpg"));
    gMeshComponentSystem[0].mTexture = CreateTexture(GenerateMagnitudeHeatmap(vecField, 127, 127));
    gMeshComponentSystem[0].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[0]);
    free(vecField);

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