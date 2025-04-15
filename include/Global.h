#pragma once
#include <iostream>
#include <string.h>
#include <stdint.h>

#define USE_GPU

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILE_AND_LINE __FILE__ ": Line " TOSTRING(__LINE__)
