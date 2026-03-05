#pragma once
#include "Components.h"

/* --- SHADER --- */
typedef struct UniformBufferObject {
    __attribute__((aligned(16))) mat4 model;
    __attribute__((aligned(16))) mat4 view;
    __attribute__((aligned(16))) mat4 proj;
}UniformBufferObject;
/* --- SHADER --- */

/* --- MESH --- */
extern MeshComponent* gMeshComponentSystem;
extern int gNumMeshComponents;
typedef enum{
    MESHTYPE_PLANE,
    MESHTYPE_CUBE,
    MESHTYPE_SPHERE,
    MESHTYPE_PYRAMID
}MeshType;

void InitMeshSystem();
void AddMesh(MeshComponent _mesh);
MeshComponent CreateMeshComponent(int _id, MeshType _meshType);
void CleanupMesh();
/* --- MESH --- */

/* --- TRANSFORM --- */
// extern ComponentTransform* gTransformSystem;

// void InitTransformSystem();
// ComponentMesh CreateTransformComponent(int _id);
// void CleanupTransform();
/* --- TRANSFORM --- */