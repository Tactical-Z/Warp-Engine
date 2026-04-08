
#include "UtilMath.h"
#include "Logger.h"

uint8_t* GenerateMagnitudeHeatmap(VectorField field) {

    size_t width = field.mWidth;
    size_t height = field.mHeight;
    size_t total = width * height;
    uint8_t* image = malloc(total * 4); // RGBA

    if (!image) return NULL;
    
    // First pass: find max magnitude
    float maxMag = 0.0f;
    for (size_t i = 0; i < total; i++) {
        float vx = field.mVectorField[i][0];
        float vy = field.mVectorField[i][1];
        float mag = sqrtf(vx * vx + vy * vy);
        if (mag > maxMag) maxMag = mag;
    }

    // Second pass: fill image with 90° clockwise rotation
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {

            size_t origIndex = y * width + x;
            float vx = field.mVectorField[origIndex][0];
            float vy = field.mVectorField[origIndex][1];
            float mag = sqrtf(vx * vx + vy * vy);
            float normalized = mag / maxMag;
            uint8_t c = (uint8_t)(normalized * 255.0f);

            // Rotated index in flat array
            size_t rotatedIndex = x * height + (height - 1 - y);

            image[rotatedIndex * 4 + 0] = c;
            image[rotatedIndex * 4 + 1] = c;
            image[rotatedIndex * 4 + 2] = c;
            image[rotatedIndex * 4 + 3] = 255;
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

    // =========================
    // PASS 1: compute max curl
    // =========================
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

    // =========================
    // PASS 2: generate image with 90° clockwise rotation
    // =========================
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

            // Rotated index: 90° clockwise
            size_t rotatedIndex = x * height + (height - 1 - y);

            // Colors: positive curl = red, negative curl = blue
            image[rotatedIndex * 4 + 0] = c;         // Red
            image[rotatedIndex * 4 + 1] = 0;         // Green
            image[rotatedIndex * 4 + 2] = 255 - c;   // Blue
            image[rotatedIndex * 4 + 3] = 255;       // Alpha
        }
    }

    return image;
}