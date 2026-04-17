
#include "UtilMath.h"
#include "Logger.h"

float Distance2(vec2 _a, vec2 _b) {
    float dx = _a[0] - _b[0];
    float dy = _a[1] - _b[1];
    return dx * dx + dy * dy;
}

float* GenerateNoiseMap(size_t _width, size_t _height) {

    float* noise = malloc(sizeof(float) * _width * _height);
    if (!noise) {
        LOG_ERROR("Fail to allocate nosie map memory");
        return NULL;
    }

    for (size_t i = 0; i < _width * _height; i++) {
        noise[i] = (float)rand() / RAND_MAX;
    }

    return noise;
}
    
uint8_t* GenerateImageFromNoise(float* _noise, size_t _width, size_t _height) {

    uint8_t* image = malloc(_width * _height * 4);
    if (!image || !_noise) {
        LOG_ERROR("Fail to allocate image memory");
        return NULL;
    }

    for (size_t i = 0; i < _width * _height; i++) {
        uint8_t c = (uint8_t)(_noise[i] * 255.0f);

        image[i*4 + 0] = c;
        image[i*4 + 1] = c;
        image[i*4 + 2] = c;
        image[i*4 + 3] = 255;
    }

    free(_noise);
    return image;
}

float ComputeLICPixel(VectorField* _field, float* _noise, float _stepSize, 
    float _maxSteps, IntegratorNormalization _noramlization, int _width, int _height, float _x, float _y)
{
    float sum = 0.0f;
    float weightSum = 0.0f;

    // center sample
    int ix = (int)_x;
    int iy = (int)_y;
    sum += _noise[iy * _width + ix];
    weightSum += 1.0f;

    // forward + backward
    for (int dir = -1; dir <= 1; dir += 2)
    {
        float px = _x;
        float py = _y;

        for (int i = 0; i < _maxSteps; i++)
        {
            vec2 fieldVector;
            if(_noramlization == NORMALIZE){
                GetNormalizedFieldSample(_field, px, py, &fieldVector);
            } else {
                SampleField(_field, px, py, &fieldVector);
            }
            

            if (fabsf(fieldVector[0]) < 1e-6f && fabsf(fieldVector[1]) < 1e-6f)
                break;

            px += fieldVector[0] * _stepSize * dir;
            py += fieldVector[1] * _stepSize * dir;

            if (px < 0 || py < 0 || px >= _width || py >= _height)
                break;

            int sx = (int)px;
            int sy = (int)py;

            float sample = _noise[sy * _width + sx];

            sum += sample;
            weightSum += 1.0f;
        }
    }

    return sum / weightSum;
}

float* GenerateLICImage(VectorField* _field, float* _noise, float _stepSize, 
    float _maxSteps, IntegratorNormalization _noramlization)
{
    size_t width = _field->mWidth;
    size_t height = _field->mHeight;

    float* lic = malloc(sizeof(float) * width * height);
    if (!lic) return NULL;

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            size_t idx = y * width + (width - 1 - x);
            lic[idx] = ComputeLICPixel(_field, _noise, _stepSize, _maxSteps, 
                                        _noramlization, width, height, (float)x, (float)y);
        }
    }

    return lic;
}

uint8_t* GenerateMagnitudeHeatmap(VectorField field) {

    size_t width = field.mWidth;
    size_t height = field.mHeight;
    size_t total = width * height;
    uint8_t* image = malloc(total * 4); // RGBA

    if (!image) return NULL;
    
    // find max magnitude
    float maxMag = 0.0f;
    for (size_t i = 0; i < total; i++) {
        float vx = field.mVectorField[i][0];
        float vy = field.mVectorField[i][1];
        float mag = sqrtf(vx * vx + vy * vy);
        if (mag > maxMag) maxMag = mag;
    }

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {

            size_t origIndex = y * width + x;
            float vx = field.mVectorField[origIndex][0];
            float vy = field.mVectorField[origIndex][1];
            float mag = sqrtf(vx * vx + vy * vy);
            float normalized = mag / maxMag;
            uint8_t c = (uint8_t)(normalized * 255.0f);

            size_t idx = y * width + (width - 1 - x);

            image[idx * 4 + 0] = c;
            image[idx * 4 + 1] = c;
            image[idx * 4 + 2] = c;
            image[idx * 4 + 3] = 255;
        }
    }

    return image;
}

uint8_t* GenerateVorticityHeatmap(VectorField field) {

    size_t width = field.mWidth;
    size_t height = field.mHeight;
    size_t total = width * height;
    uint8_t* image = malloc(total * 4); // RGBA
    if (!image) return NULL;

    // Initialize to black
    memset(image, 0, total * 4);

    float maxAbsCurl = 0.0f;

    // compute max curl
    for (size_t y = 1; y < height - 1; y++) {
        for (size_t x = 1; x < width - 1; x++) {

            float vx_up    = field.mVectorField[(y+1)*width + x][0];
            float vx_down  = field.mVectorField[(y-1)*width + x][0];

            float vy_right = field.mVectorField[y*width + (x+1)][1];
            float vy_left  = field.mVectorField[y*width + (x-1)][1];

            float dVy_dx = (vy_right - vy_left) * 0.5f;
            float dVx_dy = (vx_up - vx_down) * 0.5f;

            float curl = dVy_dx - dVx_dy;

            float absCurl = fabsf(curl);
            if (absCurl > maxAbsCurl)
                maxAbsCurl = absCurl;
        }
    }

    if (maxAbsCurl < 1e-6f)
        maxAbsCurl = 1.0f;


    // generate image with 90° clockwise rotation
    for (size_t y = 1; y < height - 1; y++) {
        for (size_t x = 1; x < width - 1; x++) {

            float vx_up    = field.mVectorField[(y+1)*width + x][0];
            float vx_down  = field.mVectorField[(y-1)*width + x][0];

            float vy_right = field.mVectorField[y*width + (x+1)][1];
            float vy_left  = field.mVectorField[y*width + (x-1)][1];

            float dVy_dx = (vy_right - vy_left) * 0.5f;
            float dVx_dy = (vx_up - vx_down) * 0.5f;

            float curl = dVy_dx - dVx_dy;

            // Normalize to [0,1] (0.5 = zero curl)
            float normalized = (curl / maxAbsCurl) * 0.5f + 0.5f;
            if (normalized < 0.0f) normalized = 0.0f;
            if (normalized > 1.0f) normalized = 1.0f;

            uint8_t c = (uint8_t)(normalized * 255.0f);

            size_t idx = y * width + (width - 1 - x);

            // Colors: positive curl = red, negative curl = blue
            image[idx * 4 + 0] = c;         // Red
            image[idx * 4 + 1] = 0;         // Green
            image[idx * 4 + 2] = 255 - c;   // Blue
            image[idx * 4 + 3] = 255;       // Alpha
        }
    }

    return image;
}

void ConvertPointsToNDC(vec2* _points, int _count, float _width, float _height)
{
    float ndcMargin = NDC_MARGIN;
    for (int i = 0; i < _count; i++)
    {
        float x = _points[i][0];
        float y = _points[i][1];

        float ndcX = ((x / _width) * 2.0f - 1.0f) * ndcMargin;
        float ndcY = ((y / _height) * 2.0f - 1.0f) * ndcMargin;

        _points[i][0] = ndcX;
        _points[i][1] = ndcY;
    }
}

void SampleField(VectorField* _field, float _x, float _y, vec2* _out)
{
    int x0 = (int)floorf(_x);
    int y0 = (int)floorf(_y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Bounds check
    if (x0 < 0 || y0 < 0 || x1 >= _field->mWidth || y1 >= _field->mHeight){
        (*_out)[0] = 0.0f;
        (*_out)[1] = 0.0f;
        return;
    }
        
    float tx = _x - x0;
    float ty = _y - y0;

    vec2 v00, v10, v01, v11;

    glm_vec2_copy(_field->mVectorField[y0 * _field->mWidth + x0], v00);
    glm_vec2_copy(_field->mVectorField[y0 * _field->mWidth + x1], v10);
    glm_vec2_copy(_field->mVectorField[y1 * _field->mWidth + x0], v01);
    glm_vec2_copy(_field->mVectorField[y1 * _field->mWidth + x1], v11);

    vec2 v0 = {
        v00[0] * (1 - tx) + v10[0] * tx,
        v00[1] * (1 - tx) + v10[1] * tx
    };

    vec2 v1 = {
        v01[0] * (1 - tx) + v11[0] * tx,
        v01[1] * (1 - tx) + v11[1] * tx
    };

    (*_out)[0] = v0[0] * (1 - ty) + v1[0] * ty;
    (*_out)[1] = v0[1] * (1 - ty) + v1[1] * ty;
}

void GetNormalizedFieldSample(VectorField* _field, float _x, float _y, vec2* _out)
{
    vec2 fieldVector;
    SampleField(_field, _x, _y, &fieldVector);
    float len = glm_vec2_norm(fieldVector);

    if (len < 1e-8f) {
        (*_out)[0] = 0.0f;
        (*_out)[1] = 0.0f;
        return;
    }

    (*_out)[0] = fieldVector[0] / len;
    (*_out)[1] = fieldVector[1] / len;
}

vec2* GenerateFieldlineEuler(VectorField* _field, vec2 _seed, float _stepSize, 
    int _maxSteps, int* _outCount, IntegratorNormalization _normalization)
{
    vec2* points = malloc(sizeof(vec2) * _maxSteps);
    if (!points){
        return NULL;
    } 

    // Start at seed location
    vec2 point = {_seed[0], _seed[1]};

    // number of points
    int count = 0;
    for (int i = 0; i < _maxSteps; i++)
    {
        // Stop if outside field
        if (point[0] < 0 || 
            point[1] < 0 ||
            point[0] >= _field->mWidth ||
            point[1] >= _field->mHeight)
        {
            break;
        }
        
        // Store point
        glm_vec2_copy(point, points[count++]);

        // Sample vector field
        vec2 fieldVector = {0};
        SampleField(_field, point[0], point[1], &fieldVector);
        float lenOfFieldVec = glm_vec2_norm(fieldVector);

        // Stop if near zero (dead zone)
        if (lenOfFieldVec < 1e-5f)
            break;

        if(_normalization == NORMALIZE){
            fieldVector[0] /= lenOfFieldVec;
            fieldVector[1] /= lenOfFieldVec;
        }

        // Euler step
        point[0] += _stepSize * fieldVector[0];
        point[1] += _stepSize * fieldVector[1];
    }

    *_outCount = count;
    return points;
}

vec2* GenerateFieldlineRK4(VectorField* _field, vec2 _seed, float _stepSize, int _maxSteps, int* _outCount, IntegratorNormalization _normalization)
{
    vec2* points = malloc(sizeof(vec2) * _maxSteps);
    if (!points){
        return NULL;
    }
    vec2 point = { _seed[0], _seed[1] };
    int count = 0;
    for (int i = 0; i < _maxSteps; i++)
    {
        // bounds check
        if (point[0] < 0 ||  point[1] < 0 || point[0] >= _field->mWidth || point[1] >= _field->mHeight){
            break;
        }
        glm_vec2_copy(point, points[count++]);

        vec2 k1 = {0}, k2 = {0}, k3 = {0}, k4 = {0};
        vec2 temp = {0};
        // k1
        if(_normalization == NORMALIZE){
            GetNormalizedFieldSample(_field, point[0], point[1], &k1);
        } else {
            SampleField(_field, point[0], point[1], &k1);
        }
        // k2
        temp[0] = point[0] + 0.5f * _stepSize * k1[0];
        temp[1] = point[1] + 0.5f * _stepSize * k1[1];
        if(_normalization == NORMALIZE){
            GetNormalizedFieldSample(_field, temp[0], temp[1], &k2);
        } else {
            SampleField(_field, temp[0], temp[1], &k2);
        }
        // k3
        temp[0] = point[0] + 0.5f * _stepSize * k2[0];
        temp[1] = point[1] + 0.5f * _stepSize * k2[1];
        if(_normalization == NORMALIZE){
            GetNormalizedFieldSample(_field, temp[0], temp[1], &k3);
        } else {
            SampleField(_field, temp[0], temp[1], &k3);
        }
        // k4
        temp[0] = point[0] + _stepSize * k3[0];
        temp[1] = point[1] + _stepSize * k3[1];
        if(_normalization == NORMALIZE){
            GetNormalizedFieldSample(_field, temp[0], temp[1], &k4);
        } else {
            SampleField(_field, temp[0], temp[1], &k4);
        }        
        // RK4 combine
        vec2 delta = {0};
        delta[0] = (k1[0] + 2*k2[0] + 2*k3[0] + k4[0]) / 6.0f;
        delta[1] = (k1[1] + 2*k2[1] + 2*k3[1] + k4[1]) / 6.0f;

        point[0] += _stepSize * delta[0];
        point[1] += _stepSize * delta[1];
    }

    *_outCount = count;
    return points;
}

vec2* GenerateFullFieldLine(VectorField* _field, vec2 _seed, float _stepSize, int _maxSteps, int* _outCount, IntegratorType _integratorType, IntegratorNormalization _normalization){

    int countFront = 0;
    int countBack = 0;
    float stepSizeFront = _stepSize;
    float stepSizeBack = _stepSize * -1.f;

    vec2* pointsFront;
    vec2* pointsBack;

    switch (_integratorType)
    {
    case INTEGRATOR_EULER:
        pointsFront = GenerateFieldlineEuler(_field, _seed, stepSizeFront, _maxSteps, &countFront, _normalization);
        pointsBack = GenerateFieldlineEuler(_field, _seed, stepSizeBack, _maxSteps, &countBack, _normalization);
        break;
    case INTEGRATOR_4RK:
        pointsFront = GenerateFieldlineRK4(_field, _seed, stepSizeFront, _maxSteps, &countFront, _normalization);
        pointsBack = GenerateFieldlineRK4(_field, _seed, stepSizeBack, _maxSteps, &countBack, _normalization);
        break;
    default:
        break;
    }

    // Seed out of bounds
    if(!pointsFront && !pointsBack){
        return NULL;
    }

    int fullCount = countFront + countBack - 1;
    
    vec2* fullFieldLine = malloc(sizeof(vec2) * fullCount);
    if(!fullFieldLine){
        LOG_ERROR("Full Field Line array memory allocation failure.");
        return NULL;
    }

    int idx = 0;

    // Reverse backward
    if(pointsBack){
        for (int i = countBack - 1; i >= 1; i--){
            glm_vec2_copy(pointsBack[i], fullFieldLine[idx++]);
        }
    }
    
    if(pointsFront){
        for (int i = 0; i < countFront; i++){
            glm_vec2_copy(pointsFront[i], fullFieldLine[idx++]);
        }
    }

    *_outCount = fullCount;
    ConvertPointsToNDC(fullFieldLine, fullCount, (float)_field->mWidth, (float)_field->mHeight);

    free(pointsFront);
    free(pointsBack);
    return fullFieldLine;
}

vec2* GenerateDensityBasedSeeds(VectorField* _field, int _seedCount)
{
    if (!_field || _seedCount <= 0)
        return NULL;

    vec2* seeds = malloc(sizeof(vec2) * _seedCount);
    if (!seeds) {
        LOG_ERROR("Seed allocation failed");
        return NULL;
    }

    int width  = _field->mWidth;
    int height = _field->mHeight;

    float minDist = sqrtf((width * height) / (float)_seedCount);
    float minDist2 = minDist * minDist;

    int count = 0;
    int maxAttempts = _seedCount * 10;

    for (int attempts = 0; attempts < maxAttempts && count < _seedCount; attempts++)
    {
        vec2 candidate;

        candidate[0] = ((float)rand() / RAND_MAX) * (width - 1);
        candidate[1] = ((float)rand() / RAND_MAX) * (height - 1);

        // Check distance against all existing seeds
        int valid = 1;

        for (int i = 0; i < count; i++) {
            if (Distance2(candidate, seeds[i]) < minDist2) {
                valid = 0;
                break;
            }
        }

        if (valid) {
            seeds[count][0] = candidate[0];
            seeds[count][1] = candidate[1];
            count++;
        }
    }

    if (count == 0) {
        free(seeds);
        return NULL;
    }

    return seeds;
}

vec2* GenerateUniformBasedSeeds(VectorField* _field, int _seedCount) {
    
    if (!_field || _seedCount <= 0){
        return NULL;
    }
    
    vec2* seeds = (vec2*)malloc(sizeof(vec2) * _seedCount);
    if (!seeds){
        LOG_ERROR("Seed position array memory allocation faild.");
        return NULL;
    }

    int width  = _field->mWidth;
    int height = _field->mHeight;

    // Compute grid dimensions (Nx * Ny ≈ seedCount)
    int nx = (int)sqrtf((float)_seedCount);
    int ny = nx;

    if (nx * ny < _seedCount)
        nx++;

    float dx = (float)width  / (float)nx;
    float dy = (float)height / (float)ny;

    int i = 0;
    for (int y = 0; y < ny && i < _seedCount; y++) {
        for (int x = 0; x < nx && i < _seedCount; x++) {

            // center of each cell
            float sx = (x + 0.5f) * dx;
            float sy = (y + 0.5f) * dy;

            seeds[i][0] = sx;
            seeds[i][1] = sy;

            i++;
        }
    }

    return seeds;
}

vec2* GenerateRandomBasedSeeds(VectorField* _field, int _seedCount) {
    
    if (!_field || _seedCount <= 0){
        return NULL;
    }
    
    vec2* seeds = (vec2*)malloc(sizeof(vec2) * _seedCount);
    if (!seeds){
        LOG_ERROR("Seed position array memory allocation faild.");
        return NULL;
    }

    for (int i = 0; i < _seedCount; i++) {
        // Uniform random in [0, width)
        float x = ((float)rand() / (float)RAND_MAX) * (float)(_field->mWidth - 1);
        float y = ((float)rand() / (float)RAND_MAX) * (float)(_field->mHeight - 1);

        seeds[i][0] = x;
        seeds[i][1] = y;
    }

    return seeds;
}