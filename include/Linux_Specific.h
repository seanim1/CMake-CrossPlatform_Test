#pragma once
#ifdef __linux__

#include "Global.h"
#include <cpuid.h>  // For Linux: __get_cpuid

void queryProcessor();
#endif