#pragma once
#ifdef __linux__

#include "Global.h"
#include <fstream>
#include <unistd.h>
#include <dirent.h>
#include <cpuid.h>  // For Linux: __get_cpuid
void queryProcessor();
inline void queryProcessor_PLATFORM() {
    std::cout << "Hello, Linux!" << std::endl;
    queryProcessor();
}
#endif
