#if defined(__APPLE__)
#pragma once
#include "Global.h"
#include <sys/types.h>
#include <sys/sysctl.h>  // For sysctlbyname on macOS
void queryProcessorName();
inline void queryProcessor() {
    std::cout << "Hello, Apple MacOS!" << std::endl;
    queryProcessorName();
}
#endif
