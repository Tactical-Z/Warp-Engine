
#include "SceneManager.h"
#include "ComponentSystems.h"
#include "VKManager.h"

void SceneBegin(){
    AddMesh(CreateMeshComponent(0, MESHTYPE_PLANE));

}

void SceneUpdate(float _dt){

}

void SceneRender(GLFWwindow* _glfwWindow){
    DrawFrame(_glfwWindow);
}

void SceneCleanup(){

    CleanupMesh();
}