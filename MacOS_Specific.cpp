#if defined(__APPLE__)

#include "MacOS_Specific.h"

void queryProcessorName() {
    char cpuName[49] = { 0 };  // 48 characters + null terminator
    cpuName[48] = '\0'; // Ensure null termination
    // Get CPU name using sysctl on Apple Silicon (ARM-based Macs)
    size_t len = sizeof(cpuName);
    // sysctlbyname is used to query system information on macOS
    if (sysctlbyname("machdep.cpu.brand_string", cpuName, &len, NULL, 0) != 0) {
        std::cerr << "Error: Unable to retrieve CPU brand string from sysctl.\n";
    }
    else {
        std::cout << "CPU Name: " << cpuName << "\n";
    }
}
#endif
