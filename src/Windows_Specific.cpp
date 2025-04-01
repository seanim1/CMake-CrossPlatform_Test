#ifdef _WIN32

#include "Windows_Specific.h"

#include <intrin.h>  // For MSVC __cpuid

static void print_binary_64bits(uint64_t mask, uint8_t bitCount) {
    for (int i = bitCount - 1; i >= 0; i--) {  // Loop through all 64 bits
        printf("%d", (mask >> i) & 1);
    }
}
static CPU_CAPABILITIES queryProcessorSpecs() {
    // Get the logical processor information
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    CPU_CAPABILITIES cpuCap;
    cpuCap.logicalThreadCount = sysInfo.dwNumberOfProcessors;

    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(NULL, &bufferSize); // First call to get size

    // Allocate the buffer to store information
    BYTE* buffer = (BYTE*)malloc(bufferSize); CATCH_MALLOC_ERROR(buffer);

    CATCH_ZERO_ERROR(GetLogicalProcessorInformation((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)buffer, &bufferSize));

    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer);
    DWORD processorCount = bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

    DWORD physicalCoreCount = 0;
    long cacheSize_L1 = 0;
    // Iterate through the processors and print cache information
    for (DWORD i = 0; i < processorCount; ++i) {
        printf("ProcessorMask: ");
        print_binary_64bits(info[i].ProcessorMask, cpuCap.logicalThreadCount);
        printf(" ");
        // Print information about each processor's relationship type
        switch (info[i].Relationship) {
        case RelationProcessorCore:
            printf("<Processor Core> ");
            break;
        case RelationNumaNode:
            printf("<NUMA Node> ");
            break;
        case RelationCache:
            printf("<Cache> ");
            break;
        case RelationProcessorPackage:
            printf("<Processor Package> ");
            break;
        case RelationGroup:
            printf("<Group> ");
            break;
        case RelationAll:
            printf("<All> ");
            break;
        default:
            printf("Unknown Relationship\n");
        }

        // Print information about the cache if it's a cache relationship
        if (info[i].Relationship == RelationCache) {
            printf("Type: ");
            switch (info[i].Cache.Type) {
            case CacheData:
                printf("_______Data,");
                break;
            case CacheInstruction:
                printf("Instruction,");
                break;
            case CacheUnified:
                printf("____Unified,");
                break;
            default:
                printf("____Unknown,");
            }

            printf(" Level %d, %ld Bytes, %d-way, %d Bytes Line, %d Sets",
                info[i].Cache.Level, info[i].Cache.Size, info[i].Cache.Associativity, info[i].Cache.LineSize, info[i].Cache.Size / (info[i].Cache.Associativity * info[i].Cache.LineSize));
            if (info[i].Cache.Level == 1) {
                cpuCap.L1cacheSize_Byte = info[i].Cache.Size;
                cpuCap.L1cacheLineSize_Byte = info[i].Cache.LineSize;
            }
            else if (info[i].Cache.Level == 2) {
                cpuCap.L2cacheSize_Byte = info[i].Cache.Size;
            }
            else if (info[i].Cache.Level == 3) {
                cpuCap.L3cacheSize_Byte = info[i].Cache.Size;
            }
            // We have 8-way set associativity (8 banks), meaning, a set can have 8 cache lines. 
            // Higher Set associativity decreases cache misses. Lower set associatativity has faster access, but high conflict misses.
            // Index = (Address / Cache Line Size) % Number of Sets
            // Set-associative conflict occurs when multiple memory addresses (or data) map to the same set in the cache
            // I think sets in set-associative caches are analogous to banks in GPU.
            // If many threads access the same bank -> bank conflict, serialized access, lower performance.
            // If multiple memory addresses map to the same set -> Set conflicts, cache thrashing (eviction)
        }
        // Information about the processor core
        else if (info[i].Relationship == RelationProcessorCore) {
            physicalCoreCount++;
            // Information about the processor core
            // Flags. If the Relationship member of the RelationProcessorCore, this member is LTP_PC_SMT if the core has more than one logical processor, or 0 if the core has one logical processor
            if (info[i].ProcessorCore.Flags & LTP_PC_SMT) {
                printf("SMT (Hyper-Threading) enabled");
                cpuCap.is_SMT_enabled = 1;
            }
            else {
                printf("SMT (Hyper-Threading) disabled");
            }
        }
        else if (info[i].Relationship == RelationNumaNode) {
            // NUMA is literally nothing special, it just means each core has a local memory close to the core.
            // But I do have to make sure that a workthread is totally bound to a respective core.
            // on NUMA systems, you need to ensure that threads are affinity-bound to specific cores (or NUMA nodes), and that data is located locally in memory attached to those cores
            printf("NUMA Node %u", info[i].NumaNode.NodeNumber);
        }
        printf("\n");  // For readability between processor details
    }
    cpuCap.physicalCoreCount = physicalCoreCount;
    return cpuCap;
}

static void queryProcessorName() {
    int cpuInfo[4] = { 0 };  // Initialize to 0
    // Get vendor string
    __cpuid(cpuInfo, 0x0);  // CPUID function 0: Get vendor string

    char vendor[13];  // Vendor string is 12 characters + null terminator
    memcpy(vendor, &cpuInfo[1], 4);  // Copy EBX
    memcpy(vendor + 4, &cpuInfo[3], 4);  // Copy EDX
    memcpy(vendor + 8, &cpuInfo[2], 4);  // Copy ECX
    vendor[12] = '\0';  // Null-terminate string
    std::cout << "CPU Vendor: " << vendor << "\n";

    char cpuName[49] = { 0 };  // 48 characters + null terminator
    // Get CPU name (function 0x80000002 to 0x80000004)
    for (int i = 0; i < 3; i++) {
        __cpuid(cpuInfo, 0x80000002 + i);
        memcpy(cpuName + i * 16, cpuInfo, 16);  // Copy 16 bytes per call
    }
    cpuName[48] = '\0'; // Ensure null termination
    printf("CPU Name: %s\n", cpuName);
}

CPU_CAPABILITIES queryProcessor_PLATFORM() {
    std::cout << "Hello, Windows OS!" << std::endl;
    queryProcessorName();
    return queryProcessorSpecs();
}

#endif
