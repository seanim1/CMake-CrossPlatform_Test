#include "Global.h"

#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <intrin.h>  // For MSVC __cpuid
#elif __linux__
#include <cpuid.h>  // For Linux
#elif __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>  // For sysctlbyname on macOS
#endif
void queryProcessorName() {
    char cpuName[49] = { 0 };  // 48 characters + null terminator
#ifdef _WIN32 || __linux__
    int cpuInfo[4] = { 0 };  // Initialize to 0
    // Get vendor string
#ifdef _WIN32
    __cpuid(cpuInfo, 0x0);  // CPUID function 0: Get vendor string
#elif __linux__
    __get_cpuid(0x0, (unsigned int*)&cpuInfo[0], (unsigned int*)&cpuInfo[1], (unsigned int*)&cpuInfo[2], (unsigned int*)&cpuInfo[3]);
#endif
    char vendor[13];  // Vendor string is 12 characters + null terminator
    memcpy(vendor, &cpuInfo[1], 4);  // Copy EBX
    memcpy(vendor + 4, &cpuInfo[3], 4);  // Copy EDX
    memcpy(vendor + 8, &cpuInfo[2], 4);  // Copy ECX
    vendor[12] = '\0';  // Null-terminate string
    std::cout << "CPU Vendor: " << vendor << "\n";

    // Get CPU name (function 0x80000002 to 0x80000004)
    for (int i = 0; i < 3; i++) {
#ifdef _WIN32
        __cpuid(cpuInfo, 0x80000002 + i);
#elif __linux__
        __get_cpuid(0x80000002 + i, (unsigned int*)&cpuInfo[0], (unsigned int*)&cpuInfo[1], (unsigned int*)&cpuInfo[2], (unsigned int*)&cpuInfo[3]);
#endif
        memcpy(cpuName + i * 16, cpuInfo, 16);  // Copy 16 bytes per call
    }
    cpuName[48] = '\0'; // Ensure null termination
    printf("CPU Name: %s\n", cpuName);
#endif
#if defined(__APPLE__)
    // Get CPU name using sysctl on Apple Silicon (ARM-based Macs)
    size_t len = sizeof(cpuName);
    // sysctlbyname is used to query system information on macOS
    if (sysctlbyname("machdep.cpu.brand_string", cpuName, &len, NULL, 0) != 0) {
        std::cerr << "Error: Unable to retrieve CPU brand string from sysctl.\n";
    }
    else {
        std::cout << "CPU Name: " << cpuName << "\n";
    }
#endif
}

int main() {
#ifdef _WIN32
    std::cout << "Hello, Win32!" << std::endl;
#elif __linux__
    std::cout << "Hello, Linux!" << std::endl;
#elif __APPLE__
    std::cout << "Hello, Apple!" << std::endl;
#endif
    queryProcessorName();
    return 0;
}
