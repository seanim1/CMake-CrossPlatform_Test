#ifdef _WIN32
#pragma once

#include "Global.h"
#include <windows.h>
void queryProcessor();
inline void queryProcessor_crossPlatform() {
    std::cout << "Hello, Windows OS!" << std::endl;
    queryProcessor();
}
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

#endif
