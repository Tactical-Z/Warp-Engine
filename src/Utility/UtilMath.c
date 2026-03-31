
#include "UtilMath.h"
#include "Logger.h"

uint8_t* GenerateMagnitudeHeatmap(VectorField field) {

    size_t width = field.mWidth;
    size_t height = field.mHeight;
    size_t total = width * height;
    uint8_t* image = malloc(total * 4); // RGBA

    if (!image) return NULL;
    
    // First pass: find max magnitude (for normalization)
    float maxMag = 0.0f;
    for (size_t i = 0; i < total; i++) {
        float vx = field.mVectorField[i][0];
        float vy = field.mVectorField[i][1];
        float mag = sqrtf(vx * vx + vy * vy);
        if (mag > maxMag) maxMag = mag;
    }

    // Second pass: fill image
    for (size_t i = 0; i < total; i++) {
        float vx = field.mVectorField[i][0];
        float vy = field.mVectorField[i][1];
        float mag = sqrtf(vx * vx + vy * vy);

        float normalized = mag / maxMag;

        uint8_t c = (uint8_t)(normalized * 255.0f);

        image[i * 4 + 0] = c;   // R
        image[i * 4 + 1] = c;   // G
        image[i * 4 + 2] = c;   // B
        image[i * 4 + 3] = 255; // A
    }

    return image;
}

uint8_t* GenerateVorticityHeatmap(VectorField field) {

    size_t width = field.mWidth;
    size_t height = field.mHeight;
    size_t total = width * height;
    uint8_t* image = malloc(total * 4); // RGBA
    if (!image) return NULL;

    // Optional: initialize to black (handles borders too)
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

    // Prevent division by zero
    if (maxAbsCurl < 1e-6f)
        maxAbsCurl = 1.0f;

    // =========================
    // PASS 2: generate image
    // =========================
    for (size_t y = 1; y < height - 1; y++) {
        for (size_t x = 1; x < width - 1; x++) {

            size_t i = y * width + x;

            float vx_up    = field.mVectorField[(y+1)*width + x][0];
            float vx_down  = field.mVectorField[(y-1)*width + x][0];

            float vy_right = field.mVectorField[y*width + (x+1)][1];
            float vy_left  = field.mVectorField[y*width + (x-1)][1];

            float dVy_dx = (vy_right - vy_left) * 0.5f;
            float dVx_dy = (vx_up - vx_down) * 0.5f;

            float curl = dVy_dx - dVx_dy;

            // Normalize to [0,1]
            float normalized = (curl / maxAbsCurl) * 0.5f + 0.5f;

            // Clamp (important for safety)
            if (normalized < 0.0f) normalized = 0.0f;
            if (normalized > 1.0f) normalized = 1.0f;

            uint8_t c = (uint8_t)(normalized * 255.0f);

            image[i*4+0] = c;         // Red (positive curl)
            image[i*4+1] = 0;
            image[i*4+2] = 255 - c;   // Blue (negative curl)
            image[i*4+3] = 255;
        }
    }

    return image;
}