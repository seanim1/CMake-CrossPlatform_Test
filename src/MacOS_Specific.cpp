#if defined(__APPLE__)

#include "MacOS_Specific.h"
void query_sysctl_info(const char* name) {
    size_t len = 0;
    if (sysctlbyname(name, NULL, &len, NULL, 0) == 0 && len > 0) {
        std::vector<char> buffer(len);
        if (sysctlbyname(name, buffer.data(), &len, NULL, 0) == 0) {
            if (len == sizeof(int)) {
                std::cout << name << ": " << *reinterpret_cast<int*>(buffer.data()) << "\n";
            } else if (len == sizeof(long)) {
                std::cout << name << ": " << *reinterpret_cast<long*>(buffer.data()) << "\n";
            } else {
                std::cout << name << ": (Unknown Type)\n";
            }
        }
    }
}

void query_processor_info() {
    const char* general_params[] = {
        "hw.activecpu", "hw.ncpu", "hw.logicalcpu", "hw.logicalcpu_max",
        "hw.physicalcpu", "hw.physicalcpu_max", "hw.l1dcachesize", "hw.l1icachesize",
        "hw.l2cachesize", "hw.l3cachesize", "hw.cachelinesize", "hw.memsize", "hw.pagesize"
    };
    
    for (const char* param : general_params) {
        query_sysctl_info(param);
    }

    // Query performance level information
    int nperflevels = 0;
    size_t len = sizeof(nperflevels);
    if (sysctlbyname("hw.nperflevels", &nperflevels, &len, NULL, 0) == 0) {
        std::cout << "hw.nperflevels: " << nperflevels << "\n";
        for (int i = 0; i < nperflevels; ++i) {
            std::string prefix = "hw.perflevel" + std::to_string(i) + ".";
            const char* perf_params[] = {
                "cpusperl2", "cpusperl3", "l1dcachesize", "l1icachesize",
                "l2cachesize", "l3cachesize", "logicalcpu", "logicalcpu_max",
                "physicalcpu", "physicalcpu_max"
            };
            for (const char* param : perf_params) {
                query_sysctl_info((prefix + param).c_str());
            }
        }
    }
}

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
    query_processor_info();
}
#endif
