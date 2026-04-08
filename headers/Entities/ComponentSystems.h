#pragma once

#include "Components.h"
#include "HDF5Reader.h"

/* --- SHADER --- */
typedef struct {
    __attribute__((aligned(16))) mat4 view;
    __attribute__((aligned(16))) mat4 proj;
}UniformBufferObject;
/* --- SHADER --- */

/* --- MESH --- */
extern MeshComponent* gMeshComponentSystem;
extern int gNumMeshComponents;
typedef enum{
    MESHTYPE_PLANE,
    MESHTYPE_PLANE_WINDOW,
    MESHTYPE_CUBE,
    MESHTYPE_SPHERE,
    MESHTYPE_PYRAMID
}MeshType;

void InitMeshSystem();
void AddMeshComponent(MeshComponent _mesh);
MeshComponent CreateMeshComponent(int _id, MeshType _meshType);
MeshComponent GenerateVectorFieldMeshComponent(int _id, VectorField _vectorField, float _vectorLength, float _resolutionScale, int _showMagnitude);
void CleanupMesh();
/* --- MESH --- */

/* --- TRANSFORM --- */
extern TransformComponent* gTransformComponentSystem;
extern int gNumTransformComponents;

void InitTransformSystem();
void AddTransformComponent(TransformComponent _transformComp);
TransformComponent CreateTransformComponent(int _id);

void SetPosition(int _id, vec3 _pos);
void SetRotation(int _id, vec3 _rot);
void SetScale(int _id, vec3 _scale);

float* GetPosition(int _id);
float* GetRotation(int _id);
float* GetScale(int _id);

void CleanupTransform();
/* --- TRANSFORM --- */