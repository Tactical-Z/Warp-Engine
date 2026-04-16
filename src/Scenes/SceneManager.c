
#include "SceneManager.h"
#include "ComponentSystems.h"
#include "VKManager.h"
#include "VkDescriptorSetBuffer.h"
#include "Texture.h"
#include "FileSystem.h"


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
    //gMeshComponentSystem[0].mTexture = CreateTexture(GenerateMagnitudeHeatmap(vecField),vecField.mWidth, vecField.mHeight, VK_FORMAT_R8G8B8A8_SRGB);
    //gMeshComponentSystem[0].mTexture = CreateTexture(GenerateVorticityHeatmap(vecField),vecField.mWidth, vecField.mHeight, VK_FORMAT_R8G8B8A8_SRGB);
    //gMeshComponentSystem[0].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[0]);
    
    // Vector field
    //AddMeshComponent(GenerateVectorFieldMeshComponent(0, vecField, 0.03f, 0.1, 0));
    //AddTransformComponent(CreateTransformComponent(0));
    //gMeshComponentSystem[0].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[0]);
    
    // Field Lines
    //GenerateFieldLinesFromVectorField(&vecField, 0.1, 5000, INTEGRATOR_EULER, NORMALIZE, 500, UNIFORM, 1);

    // LIC
    AddMeshComponent(CreateMeshComponent(0, MESHTYPE_PLANE_WINDOW));
    //gMeshComponentSystem[0].mTexture = CreateTexture(GenerateImageFromNoise(GenerateNoiseMap(vecField.mWidth, vecField.mHeight), vecField.mWidth, vecField.mHeight),vecField.mWidth, vecField.mHeight,VK_FORMAT_R8G8B8A8_UNORM);
    gMeshComponentSystem[0].mTexture = CreateTexture(GenerateImageFromNoise(GenerateLICImage(&vecField, GenerateNoiseMap(vecField.mWidth, vecField.mHeight), 0.5, 20, NORMALIZE), vecField.mWidth, vecField.mHeight),vecField.mWidth, vecField.mHeight, VK_FORMAT_R8G8B8A8_UNORM);
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

    // // Field line 
    // vec3 color = {1, 0, 0};
    // vec2 seedPoint = {300,300};
    // int totalCount = 0;
    // vec2* fieldLinePoints = GenerateFullFieldLine(&vecField, seedPoint, 0.1, 5000, &totalCount, INTEGRATOR_EULER, NON_NORMALIZE);
    // AddMeshComponent(CreateLineMeshFromArray(1, fieldLinePoints, totalCount, color));
    // if(gMeshComponentSystem[1].mVertexCount > 0){
    //     AddTransformComponent(CreateTransformComponent(1));
    //     gMeshComponentSystem[1].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[1]);
    // }
    // free(fieldLinePoints);

    // Field line RK4
    // vec3 colornew = {0, 0, 1.0};
    // vec2 seedPointnew = {300,300};
    // int totalCountnew = 0;
    // vec2* fieldLinePointsOther = GenerateFullFieldLine(&vecField, seedPointnew, 0.1, 5000, &totalCountnew, INTEGRATOR_4RK, NON_NORMALIZE);
    // AddMeshComponent(CreateLineMeshFromArray(2, fieldLinePointsOther, totalCountnew, colornew));
    // if(gMeshComponentSystem[2].mVertexCount > 0){
    //     AddTransformComponent(CreateTransformComponent(2));
    //     gMeshComponentSystem[2].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[2]);
    // }
    // free(fieldLinePointsOther);


    free(vecField.mVectorField);
}

void GenerateFieldLinesFromVectorField(VectorField* _vecField, float _stepSize, int _maxSteps, IntegratorType _integratorType, IntegratorNormalization _normalization, int _numSeeds, SeedGenerator _seedGenerator, int _renderSeeds){

    int numFailSeeds = 0;
    vec2* seeds = NULL;
    switch (_seedGenerator)
    {
    case DENSITY:
        seeds = GenerateDensityBasedSeeds(_vecField, _numSeeds);
        break;
    case UNIFORM:
        seeds = GenerateUniformBasedSeeds(_vecField, _numSeeds);
        break;
    case RANDOM:
        seeds = GenerateRandomBasedSeeds(_vecField, _numSeeds);
        break;
    default:
        break;
    }

    int CurrentNumMeshes = GetNumMeshes();
    for (int i = 0; i < _numSeeds; i++) {
        int index = i + CurrentNumMeshes;
        int totalCount = 0;
        
        vec3 color = {1, 0, 0};
        vec2* fieldLinePoints = GenerateFullFieldLine(_vecField, seeds[i], _stepSize, _maxSteps, &totalCount, _integratorType, _normalization);
        if(!fieldLinePoints){
            LOG_ERROR("Invalid Field line");
            continue;
        }

        int meshIndex = AddMeshComponent(CreateLineMeshFromArray(index, fieldLinePoints, totalCount, color));
        if (meshIndex < 0) {
            numFailSeeds++;
            free(fieldLinePoints);
            continue;
        }
        gMeshComponentSystem[meshIndex].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[meshIndex]);
        free(fieldLinePoints);
    }
    LOG_WARN("Num out of bound seeds: %i", numFailSeeds);

    if(_renderSeeds){
        for (int i = 0; i < _numSeeds; i++) {
            vec3 color = {0, 1, 0};
            int index = i + CurrentNumMeshes;

            vec2 seed;
            glm_vec2_copy(seeds[i], seed);

            float size = 2.0f;

            vec2 vOffset = { 0.0f, size };
            vec2 hOffset = { size, 0.0f };

            vec2 v0, v1;
            vec2 h0, h1;

            // vertical line
            glm_vec2_sub(seed, vOffset, v0);
            glm_vec2_add(seed, vOffset, v1);

            // horizontal line
            glm_vec2_sub(seed, hOffset, h0);
            glm_vec2_add(seed, hOffset, h1);

            int totalCount = 4;

            vec2* fieldLinePoints = malloc(sizeof(vec2) * totalCount);

            glm_vec2_copy(v0, fieldLinePoints[0]);
            glm_vec2_copy(v1, fieldLinePoints[1]);
            glm_vec2_copy(h0, fieldLinePoints[2]);
            glm_vec2_copy(h1, fieldLinePoints[3]);
            
            ConvertPointsToNDC(fieldLinePoints, totalCount, (float)_vecField->mWidth, (float)_vecField->mHeight);

            int meshIndex = AddMeshComponent(CreateLineMeshFromArray(index, fieldLinePoints, totalCount, color));
            gMeshComponentSystem[meshIndex].mDescriptorSet = SetupMeshDescriptorSet(gVkContext.mDevice, &gMeshComponentSystem[meshIndex]);
            free(fieldLinePoints);
        }
    }
    

    free(seeds);
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