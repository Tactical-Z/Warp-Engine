
#include "HDF5Reader.h"
#include "Logger.h"

void Cleanuphdf5(hdf5MetaData* _metaData) {
    if (_metaData->mDataspace_id > 0)
        H5Sclose(_metaData->mDataspace_id);

    if (_metaData->mDatasetX_id > 0)
        H5Dclose(_metaData->mDatasetX_id);

    if (_metaData->mDatasetY_id > 0)
        H5Dclose(_metaData->mDatasetY_id);

    if (_metaData->mFile_id > 0)
        H5Fclose(_metaData->mFile_id);
}

int Openhdf5File(const char* _file, hdf5MetaData* _metaData){

    _metaData->mFile_id = H5Fopen(_file, H5F_ACC_RDONLY, H5P_DEFAULT);
    _metaData->mFilePath = _file;

    if(_metaData->mFile_id < 0){
        LOG_ERROR("Failed to open hdf5 file : %s", _file);
        return 0;
    };
    return 1;
};

int Openhdf5Datasets(const char* _xName, const char* _yName, hdf5MetaData* _metaData) {

    _metaData->mDatasetX_id = H5Dopen2(_metaData->mFile_id, _xName, H5P_DEFAULT);
    if (_metaData->mDatasetX_id < 0) {
        LOG_ERROR("Failed to open dataset: %s", _xName);
        return 0;
    }

    _metaData->mDatasetY_id = H5Dopen2(_metaData->mFile_id, _yName, H5P_DEFAULT);
    if (_metaData->mDatasetY_id < 0) {
        LOG_ERROR("Failed to open dataset: %s", _yName);
        return 0;
    }

    return 1;
}

int Openhdf5Dataspace(hdf5MetaData* _metaData) {
    _metaData->mDataspace_id = H5Dget_space(_metaData->mDatasetX_id);

    if (_metaData->mDataspace_id < 0) {
        LOG_ERROR("Failed to open dataspace");
        return 0;
    }
    return 1;
}

void Populatehdf5MetaData(hdf5MetaData* _metaData) {
    _metaData->mRank = H5Sget_simple_extent_ndims(_metaData->mDataspace_id);
    H5Sget_simple_extent_dims(_metaData->mDataspace_id, _metaData->mDimensions, NULL);
}

float* Readhdf5Dataset(hid_t _dataset_id, hdf5MetaData* _metaData){

    hsize_t total = 1;
    for (int i = 0; i < _metaData->mRank; i++)
        total *= _metaData->mDimensions[i];

    float* data = (float*)malloc(sizeof(float) * total);
    if (!data) 
        return NULL;

    if (H5Dread(_dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0) {
        free(data);
        return NULL;
    }
    return data;
};

void PrintVectorField(vec2* _vecField, hdf5MetaData* _metaData) {
    if (!_vecField || !_metaData) {
        LOG_ERROR("No vector field data to print");
        return;
    }

    size_t width  = _metaData->mDimensions[1];
    size_t height = _metaData->mDimensions[0];

    LOG_DEBUG("Printing vector field (%zu x %zu)", width, height);

    return; // FOR TESTING

    for (size_t y = 0; y < height; y++) {

        char rowBuffer[4096] = {0};
        size_t offset = 0;

        for (size_t x = 0; x < width; x++) {
            size_t i = y * width + x;

            float vx = _vecField[i][0];
            float vy = _vecField[i][1];

            offset += snprintf(rowBuffer + offset, sizeof(rowBuffer) - offset, "(%.2f, %.2f) ", vx, vy);

            // Prevent overflow
            if (offset >= sizeof(rowBuffer) - 50) {
                break;
            }
        }

        LOG_DEBUG("%s", rowBuffer);
    }
}

VectorField Readhdf5File(const char* _file, const char* _datasetX, const char* _datasetY){

    // File manipulation ---
    VectorField vectorField = {0};
    hdf5MetaData metaData = {0};
    if(!Openhdf5File(_file, &metaData))
        return vectorField;
    
    if(!Openhdf5Datasets(_datasetX, _datasetY, &metaData))
        return vectorField;
    
    if(!Openhdf5Dataspace(&metaData))
        return vectorField;
    
    Populatehdf5MetaData(&metaData);

    // Data reading ---
    float* xData = Readhdf5Dataset(metaData.mDatasetX_id, &metaData);
    float* yData = Readhdf5Dataset(metaData.mDatasetY_id, &metaData);

    size_t totalSize = metaData.mDimensions[0] * metaData.mDimensions[1];

    vectorField.mVectorField = malloc(sizeof(vec2) * totalSize);
    vectorField.mWidth = metaData.mDimensions[0];
    vectorField.mHeight = metaData.mDimensions[1];

    if (!vectorField.mVectorField) {
        free(xData);
        free(yData);
        Cleanuphdf5(&metaData);
        return vectorField;
    }

    for (size_t i = 0; i < totalSize; i++) {
        vectorField.mVectorField[i][0] = xData[i];
        vectorField.mVectorField[i][1] = yData[i];
    }

    //LOG_DEBUG("First vector: (%f, %f)", vecField[0][0], vecField[0][1]);
    //PrintVectorField(vecFieldData, &metaData);

    free(xData);
    free(yData);
    Cleanuphdf5(&metaData);

    return vectorField;
};