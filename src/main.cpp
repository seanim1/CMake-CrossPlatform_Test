#include "Global.h"

#ifdef _WIN32
#include "Windows_Specific.h"
#elif __linux__
#include "Linux_Specific.h"
#elif __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>  // For sysctlbyname on macOS
#endif
void queryProcessorNameu() {
    char cpuName[49] = { 0 };  // 48 characters + null terminator
    cpuName[48] = '\0'; // Ensure null termination
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
#ifdef __APPLE__
    std::cout << "Hello, Apple MacOS!" << std::endl;
#endif
    queryProcessor();
    queryProcessorNameu();
    return 0;
}
