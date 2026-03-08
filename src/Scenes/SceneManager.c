
#include "SceneManager.h"
#include "ComponentSystems.h"
#include "VKManager.h"

void SceneBegin(){
    AddMeshComponent(CreateMeshComponent(0, MESHTYPE_CUBE));
    //AddMeshComponent(CreateMeshComponent(0, MESHTYPE_PLANE));
    AddTransformComponent(CreateTransformComponent(0));
    //vec3 pos;
    //pos[0] = 2;
    //pos[1] = 2;
    //pos[2] = 2;
    //SetPosition(0, pos);
    //AddTransformComponent(CreateTransformComponent(0));
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