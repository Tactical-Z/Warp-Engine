#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

// ANSI color codes
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_VK      "\x1b[35m"
#define COLOR_DEFAULT "\033[0m"

// Declare a global start clock
static clock_t __log_start_clock = 0;

// Call this once at the beginning of main()
#define INIT_LOG_TIMER() (__log_start_clock = clock())

// Core log macro with elapsed time and levels
#define LOG(level, thread, color, msg, ...) do {                                   \
    clock_t __now = clock();                                                       \
    double __elapsed = (double)(__now - __log_start_clock) / CLOCKS_PER_SEC;       \
    int __sec = (int)__elapsed;                                                    \
    printf("%s[+%ds] [%s] [Thread %lu]: " msg "%s\n",                              \
        color, __sec, level, thread, ##__VA_ARGS__, COLOR_RESET);                  \
} while(0)

#define LOG_ERR(level, thread, color, msg, ...) do {                                                \
    clock_t __now = clock();                                                                        \
    double __elapsed = (double)(__now - __log_start_clock) / CLOCKS_PER_SEC;                        \
    int __sec = (int)__elapsed;                                                                     \
    fprintf(stderr, "%s[+%ds] [%s] [Thread %lu]: ", color, __sec, level, (unsigned long)(thread));  \
    fprintf(stderr, msg, ##__VA_ARGS__);                                                            \
    fprintf(stderr, ": %s%s\n", strerror(errno), COLOR_RESET);                                      \
} while(0)

// Level-specific macros
#define LOG_INFO(fmt, ...)  LOG("INFO", pthread_self(), COLOR_DEFAULT, fmt, ##__VA_ARGS__)
#define LOG_VKINFO(fmt, ...)  LOG("VkINFO", pthread_self(), COLOR_VK, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG("WARN", pthread_self(), COLOR_YELLOW, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_ERR("ERROR", pthread_self(), COLOR_RED, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG("DEBUG", pthread_self(), COLOR_CYAN, fmt, ##__VA_ARGS__)






