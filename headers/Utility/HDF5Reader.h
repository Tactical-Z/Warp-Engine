#pragma once

#include "hdf5.h"
#include "cglm.h"

typedef enum {
    DATASET_METSIM,
    DATASET_ISABEL
} DatasetType;

typedef struct{
    hid_t mFile_id; 
    hid_t mDatasetX_id;
    hid_t mDatasetY_id;
    hid_t mDataspace_id;

    int mRank;
    hsize_t mDimensions[2];

    herr_t mStatus;
    const char* mFilePath;

} hdf5MetaData;

typedef struct{
    vec2* mVectorField;
    DatasetType mDatasetType;
    size_t mWidth;
    size_t mHeight;
} VectorField;

void Cleanuphdf5(hdf5MetaData* _metaData);

int Openhdf5File(const char* _file, hdf5MetaData* _metaData);

int Openhdf5Datasets(const char* _xName, const char* _yName, hdf5MetaData* _metaData);

int Openhdf5Dataspace(hdf5MetaData* _metaData);

void Populatehdf5MetaData(hdf5MetaData* _metaData);

float* Readhdf5Dataset(hid_t _dataset_id, hdf5MetaData* _metaData);

void PrintVectorField(vec2* _vecField, hdf5MetaData* _metaData);

VectorField Readhdf5File(const char* _file, DatasetType _type, const char* _datasetX, const char* _datasetY);