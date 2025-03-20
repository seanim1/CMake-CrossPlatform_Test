#pragma once
#include <iostream>
typedef struct CPU_CAPABILITIES {
	uint8_t is_SMT_enabled;
	uint8_t logicalThreadCount;
	uint8_t physicalCoreCount;
	uint16_t L1cacheLineSize_Byte;
	uint32_t L1cacheSize_Byte;
	uint32_t L2cacheSize_Byte;
	uint32_t L3cacheSize_Byte;
} CPU_CAPABILITIES;
extern CPU_CAPABILITIES cpuCap;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILE_AND_LINE __FILE__ ": Line " TOSTRING(__LINE__)
