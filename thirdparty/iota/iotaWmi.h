#if defined (iota_WMI_IMPL)

#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__linux__)
    #if defined(__ANDROID__)
        #define PLATFORM_ANDROID
    #else
        #define PLATFORM_LINUX
    #endif
#elif defined(__unix__)
    #define PLATFORM_UNIX
#else
    #error "Unknown platform"
#endif

#include <string>
#include <vector>
#include <cstdint>

class IotaInfo {
public:
    struct CPUInfo {
        std::string vendor;
        std::string brand;
        uint32_t numCoresPhysical;
        uint32_t numCoresLogical;
        uint32_t cacheLineSize;
        uint32_t cacheL1Size;
        uint32_t cacheL2Size;
        uint32_t cacheL3Size;
        double frequencyMHz;
        double frequencyMaxMHz;
    };

    struct MemoryInfo {
        uint64_t totalPhysicalMB;
        uint64_t availablePhysicalMB;
        uint64_t totalVirtualMB;
        uint64_t availableVirtualMB;
        double loadPercent;
        uint32_t frequencyMHz;
    };

    struct GPUInfo {
        std::string name;
        std::string vendor;
        uint64_t vramTotalMB;
        bool isDiscrete;
    };

    struct OSInfo {
        std::string name;
        std::string version;
        std::string kernelVersion;
        bool is64bit;
        uint64_t uptimeSeconds;
    };

    enum class PerformanceTier {
        LOW,
        MEDIUM,
        HIGH,
        ULTRA
    };

    struct Settings {
        PerformanceTier tier;
        bool enableRayTracing;
        bool enableDLSS;
        uint32_t shadowQuality;
        uint32_t textureQuality;
        uint32_t antialiasing;
        uint32_t resolutionScale;
        uint32_t targetFPS;
        uint32_t maxVRAMBudgetMB;
        uint32_t maxRAMBudgetMB;
    };

    static CPUInfo getCPUInfo();
    static MemoryInfo getMemoryInfo();
    static std::vector<GPUInfo> getGPUInfo();
    static GPUInfo getPrimaryGPUInfo();
    static OSInfo getOSInfo();
    static PerformanceTier getPerformanceTier();
    static Settings getSettings();

private:
    IotaInfo() = delete;
};

#endif