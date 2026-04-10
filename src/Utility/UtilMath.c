
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

#include <string.h> // for memcpy
#include "cglm.h"

void SampleVectorField(VectorField* vf, float x, float y, vec2 out)
{
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    if (x0 < 0 || y0 < 0 || x1 >= vf->mWidth || y1 >= vf->mHeight) {
        glm_vec2_zero(out);
        return;
    }

    float tx = x - x0;
    float ty = y - y0;

    vec2 v00, v10, v01, v11;

    glm_vec2_copy(vf->mVectorField[y0 * vf->mWidth + x0], v00);
    glm_vec2_copy(vf->mVectorField[y0 * vf->mWidth + x1], v10);
    glm_vec2_copy(vf->mVectorField[y1 * vf->mWidth + x0], v01);
    glm_vec2_copy(vf->mVectorField[y1 * vf->mWidth + x1], v11);

    vec2 a, b;

    for (int i = 0; i < 2; i++) {
        a[i] = v00[i] * (1 - tx) + v10[i] * tx;
        b[i] = v01[i] * (1 - tx) + v11[i] * tx;
        out[i] = a[i] * (1 - ty) + b[i] * ty;
    }
}

size_t IntegrateEuler(VectorField* vf, vec2 start, float stepSize, int maxSteps, vec2* outPoints, FieldlineType flt)
{
    vec2 p;
    glm_vec2_copy(start, p);

    size_t count = 0;

    for (int i = 0; i < maxSteps; i++) {

        if (p[0] < 0 || p[1] < 0 ||
            p[0] >= vf->mWidth || p[1] >= vf->mHeight)
            break;

        glm_vec2_copy(p, outPoints[count++]);

        vec2 v;
        SampleVectorField(vf, p[0], p[1], v);

        if (glm_vec2_norm(v) == 0.0f)
            break;

        if (flt == PATH_LINE) {

            p[0] += stepSize * v[0];
            p[1] += stepSize * v[1];

        } else {

            glm_vec2_normalize_to(v, v);

            p[0] += stepSize * v[0];
            p[1] += stepSize * v[1];
        }
    }

    return count;
}

size_t IntegrateRK4(VectorField* vf, vec2 start, float h, int maxSteps, vec2* outPoints, FieldlineType flt)
{
    vec2 p;
    glm_vec2_copy(start, p);

    size_t count = 0;

    for (int i = 0; i < maxSteps; i++) {

        if (p[0] < 0 || p[1] < 0 ||
            p[0] >= vf->mWidth || p[1] >= vf->mHeight)
            break;

        glm_vec2_copy(p, outPoints[count++]);

        vec2 k1, k2, k3, k4;
        vec2 tmp;

        SampleVectorField(vf, p[0], p[1], k1);

        tmp[0] = p[0] + 0.5f * h * k1[0];
        tmp[1] = p[1] + 0.5f * h * k1[1];
        SampleVectorField(vf, tmp[0], tmp[1], k2);

        tmp[0] = p[0] + 0.5f * h * k2[0];
        tmp[1] = p[1] + 0.5f * h * k2[1];
        SampleVectorField(vf, tmp[0], tmp[1], k3);

        tmp[0] = p[0] + h * k3[0];
        tmp[1] = p[1] + h * k3[1];
        SampleVectorField(vf, tmp[0], tmp[1], k4);

        if (flt == STREAM_LINE) {
            glm_vec2_normalize_to(k1, k1);
            glm_vec2_normalize_to(k2, k2);
            glm_vec2_normalize_to(k3, k3);
            glm_vec2_normalize_to(k4, k4);
        }

        p[0] += (h / 6.0f) *
                (k1[0] + 2.0f*k2[0] + 2.0f*k3[0] + k4[0]);

        p[1] += (h / 6.0f) *
                (k1[1] + 2.0f*k2[1] + 2.0f*k3[1] + k4[1]);
    }

    return count;
}