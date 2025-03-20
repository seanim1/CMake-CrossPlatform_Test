#ifdef __linux__

#include "Linux_Specific.h"

void queryProcessorName() {
    char cpuName[49] = { 0 };  // 48 characters + null terminator
    int cpuInfo[4] = { 0 };  // Initialize to 0
    // Get vendor string
    __get_cpuid(0x0, (unsigned int*)&cpuInfo[0], (unsigned int*)&cpuInfo[1], (unsigned int*)&cpuInfo[2], (unsigned int*)&cpuInfo[3]);

    char vendor[13];  // Vendor string is 12 characters + null terminator
    memcpy(vendor, &cpuInfo[1], 4);  // Copy EBX
    memcpy(vendor + 4, &cpuInfo[3], 4);  // Copy EDX
    memcpy(vendor + 8, &cpuInfo[2], 4);  // Copy ECX
    vendor[12] = '\0';  // Null-terminate string
    std::cout << "CPU Vendor: " << vendor << "\n";

    // Get CPU name (function 0x80000002 to 0x80000004)
    for (int i = 0; i < 3; i++) {
        __get_cpuid(0x80000002 + i, (unsigned int*)&cpuInfo[0], (unsigned int*)&cpuInfo[1], (unsigned int*)&cpuInfo[2], (unsigned int*)&cpuInfo[3]);
        memcpy(cpuName + i * 16, cpuInfo, 16);  // Copy 16 bytes per call
    }
    cpuName[48] = '\0'; // Ensure null termination
    printf("CPU Name: %s\n", cpuName);
}

void queryProcessor() {
    queryProcessorName();
}
#endif
