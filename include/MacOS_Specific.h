#if defined(__APPLE__)
#pragma once
#include "Global.h"
#include <unistd.h> // for _SC_NPROCESSORS_ONLN
#include <sys/sysctl.h>  // For sysctlbyname on macOS
void queryProcessorName();
inline void queryProcessor_crossPlatform() {
    std::cout << "Hello, Apple MacOS!" << std::endl;
    queryProcessorName();
}
#endif
