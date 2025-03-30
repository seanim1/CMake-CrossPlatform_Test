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

int count_physical_cores() {
    DIR* dir;
    struct dirent* entry;
    char path[512];
    int core_ids[256] = {0}; // Assume max 256 cores
    int core_count = 0;

    // Scan CPU topology
    dir = opendir("/sys/devices/system/cpu");
    if (!dir) return -1;

    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "cpu", 3) == 0 && isdigit(entry->d_name[3])) {
            snprintf(path, sizeof(path), "/sys/devices/system/cpu/%s/topology/core_id", entry->d_name);

            FILE* file = fopen(path, "r");
            if (!file) continue;

            int core_id;
            fscanf(file, "%d", &core_id);
            fclose(file);

            // Track unique core IDs
            int found = 0;
            for (int i = 0; i < core_count; i++) {
                if (core_ids[i] == core_id) {
                    found = 1;
                    break;
                }
            }
            if (!found) core_ids[core_count++] = core_id;
        }
    }
    closedir(dir);
    return core_count;
}

long getCacheSize(int level) {
    std::string path = "/sys/devices/system/cpu/cpu0/cache/index" + std::to_string(level - 1) + "/size";
    std::ifstream file(path);

    if (!file.is_open()) return -1; // Return -1 if file read fails

    std::string sizeStr;
    file >> sizeStr;
    file.close();

    // Convert size to bytes
    long size = std::stol(sizeStr);

    return size;
}

void queryProcessor() {
    queryProcessorName();

    int logical_threads = sysconf(_SC_NPROCESSORS_ONLN);
    int physical_cores = count_physical_cores();
    long L1d = getCacheSize(1);  // L1 Data
    long L1i = getCacheSize(2);  // L1 Instruction
    long L2  = getCacheSize(3);
    long L3  = getCacheSize(4);
    long cacheLineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);  // Get Cache Line Size
    printf("Logical Thread Count: %d\n", logical_threads);
    printf("Physical Core Count:  %d\n", physical_cores);
    std::cout << "L1 Cache line size: " << cacheLineSize << " Bytes" << std::endl;
    std::cout << "L1 Data Cache: " << L1d << " KB\n";
    std::cout << "L1 Instruction Cache: " << L1i << " KB\n";
    std::cout << "L2 Cache: " << L2 << " KB\n";
    std::cout << "L3 Cache: " << L3 << " KB\n";
}
#endif
