
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Appends file name to asset directory to find the wanted asset
const char* AssetDir(const char* _fileName) {
    static char assetPath[1024] = {0};
    if (assetPath[0] == '\0') {
        char exePath[MAX_PATH] = {0};

        // Get full path to executable
        GetModuleFileNameA(NULL, exePath, MAX_PATH);

        // Strip off the exe name
        char* lastSlash = strrchr(exePath, '\\');
        if (lastSlash) *lastSlash = '\0';

        // Step up one folder if executable is in "build/"
        char* secondSlash = strrchr(exePath, '\\');
        if (secondSlash) *secondSlash = '\0';

        // Build path to assets folder
        snprintf(assetPath, sizeof(assetPath), "%s\\assets\\", exePath);
    }

    // Append requested file to assets path
    static char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s%s", assetPath, _fileName);
    //LOG_DEBUG("Full path = %s", fullPath);
    return fullPath;
};