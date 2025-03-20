#pragma once
#ifdef __linux__

#include "Global.h"
#include <cpuid.h>  // For Linux: __get_cpuid
void queryProcessorName();
inline void queryProcessor() {
    std::cout << "Hello, Linux!" << std::endl;
    queryProcessorName();
}
#endif
