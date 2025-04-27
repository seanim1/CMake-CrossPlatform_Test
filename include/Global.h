#pragma once
#include <iostream>
#include <string.h>
#include <stdint.h>
#include <vector>

#define USE_GPU

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILE_AND_LINE __FILE__ ": Line " TOSTRING(__LINE__)

#if defined(_WIN32)
#define CATCH_ERROR(result)                                    \
if (result < 0) {                                                               \
    MessageBoxA(NULL, FILE_AND_LINE, "Error!", MB_ICONEXCLAMATION | MB_OK);   \
}
#define CATCH_ZERO_ERROR(result)                                    \
if (result == 0) {                                                               \
    MessageBoxA(NULL, FILE_AND_LINE, "Error (False)!", MB_ICONEXCLAMATION | MB_OK);   \
}
#define CATCH_MALLOC_ERROR(ptr)                                  \
if (ptr == NULL) {                                          \
    MessageBoxA(NULL, FILE_AND_LINE, "Malloc Failed!", MB_ICONEXCLAMATION | MB_OK); \
    exit(EXIT_FAILURE);                                      \
}
#else
    #define CATCH_ERROR(result)          (result)
    #define CATCH_ZERO_ERROR(result)     (result)
    #define CATCH_MALLOC_ERROR(ptr)       (ptr)
#endif
