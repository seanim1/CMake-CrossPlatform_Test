#include "Global.h"

#ifdef _WIN32
#include "Windows_Specific.h"
#elif __linux__
#include "Linux_Specific.h"
#elif __APPLE__
#include "MacOS_Specific.h"
#endif

int main() {
    queryProcessor_crossPlatform();
    return 0;
}
