#if defined (iota_WMI_IMPL)

#include "../iotaWmi.h"
#include <sysinfoapi.h>
#include <windows.h>
#include <intrin.h>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <vulkan/vulkan.h>

#if defined(PLATFORM_WINDOWS)

static void cpuid(int func, int subfunc, int regs[4]) {
    __cpuidex(regs, func, subfunc);
}

static std::string cpuidVendor() {
    int regs[4];
    cpuid(0, 0, regs);
    char vendor[13];
    memcpy(vendor, &regs[1], 4);
    memcpy(vendor + 4, &regs[3], 4);
    memcpy(vendor + 8, &regs[2], 4);
    vendor[12] = 0;
    return vendor;
}

static std::string cpuidBrand() {
    int regs[4];
    char brand[49];
    cpuid(0x80000000, 0, regs);
    if (regs[0] < 0x80000004) return "Unknown";
    
    cpuid(0x80000002, 0, regs);
    memcpy(brand, regs, 16);
    cpuid(0x80000003, 0, regs);
    memcpy(brand + 16, regs, 16);
    cpuid(0x80000004, 0, regs);
    memcpy(brand + 32, regs, 16);
    brand[48] = 0;
    
    std::string result = brand;
    while (!result.empty() && result[0] == ' ') result.erase(0, 1);
    return result;
}

IotaInfo::CPUInfo IotaInfo::getCPUInfo() {
    CPUInfo info = {};
    int regs[4];
    cpuid(0, 0, regs);
    char vendor[13];
    memcpy(vendor, &regs[1], 4);
    memcpy(vendor + 4, &regs[3], 4);
    memcpy(vendor + 8, &regs[2], 4);
    vendor[12] = 0;
    info.vendor = vendor;
    
    cpuid(0x80000000, 0, regs);
    if (regs[0] >= 0x80000004) {
        char brand[49] = {};
        cpuid(0x80000002, 0, (int*)brand);
        cpuid(0x80000003, 0, (int*)(brand + 16));
        cpuid(0x80000004, 0, (int*)(brand + 32));
        info.brand = brand;
        while (!info.brand.empty() && info.brand[0] == ' ') info.brand.erase(0, 1);
    } else {
        info.brand = "Unknown";
    }
    
    cpuid(0x00000001, 0, regs);
    info.cacheLineSize = ((regs[1] >> 8) & 0xFF) * 8;
    
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info.numCoresLogical = sysInfo.dwNumberOfProcessors;
    info.numCoresPhysical = 0;
    
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize);
    if (bufferSize > 0) {
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(
            bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        if (GetLogicalProcessorInformation(buffer.data(), &bufferSize)) {
            for (const auto& entry : buffer) {
                if (entry.Relationship == RelationProcessorCore) {
                    info.numCoresPhysical++;
                }
            }
        }
    }
    
    if (info.numCoresPhysical == 0) {
        info.numCoresPhysical = info.numCoresLogical;
    }
    
    info.cacheL1Size = 0;
    info.cacheL2Size = 0;
    info.cacheL3Size = 0;
    
    for (int i = 0; ; i++) {
        cpuid(0x00000004, i, regs);
        int cacheType = regs[0] & 0x1F;
        if (cacheType == 0) break;
        
        int ways = ((regs[1] >> 22) & 0x3FF) + 1;
        int partitions = ((regs[1] >> 12) & 0x3FF) + 1;
        int lineSize = (regs[1] & 0xFFF) + 1;
        int sets = regs[2] + 1;
        int sizeKB = (ways * partitions * lineSize * sets) / 1024;
        int level = (regs[0] >> 5) & 0x7;
        
        if (level == 1) {
            if (cacheType == 1 || cacheType == 2) {
                info.cacheL1Size += sizeKB;
            }
        } else if (level == 2 && cacheType == 3) {
            info.cacheL2Size += sizeKB;
        } else if (level == 3 && cacheType == 3) {
            info.cacheL3Size = sizeKB;
        }
    }
    
    if (info.numCoresPhysical > 0) {
        if (info.cacheL2Size <= 512) {
            info.cacheL2Size *= info.numCoresPhysical;
        }
        info.cacheL1Size *= info.numCoresPhysical;
    }
    
    if (info.cacheL2Size == 0 || info.cacheL3Size == 0) {
        cpuid(0x80000006, 0, regs);
        if (info.cacheL2Size == 0) info.cacheL2Size = (regs[2] >> 16) & 0xFFFF;
        if (info.cacheL3Size == 0) info.cacheL3Size = ((regs[3] >> 18) & 0x3FFF) * 512;
    }
    
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD freq = 0, size = sizeof(freq);
        RegQueryValueExA(hKey, "~MHz", nullptr, nullptr, (LPBYTE)&freq, &size);
        if (freq > 0) info.frequencyMHz = freq;
        
        size = sizeof(freq);
        RegQueryValueExA(hKey, "MaxMHz", nullptr, nullptr, (LPBYTE)&freq, &size);
        if (freq > 0) info.frequencyMaxMHz = freq;
        
        RegCloseKey(hKey);
    }
    
    if (info.frequencyMHz == 0) {
        cpuid(0x00000016, 0, regs);
        if (regs[0] > 0) info.frequencyMHz = regs[0] & 0xFFFF;
    }
    
    if (info.frequencyMaxMHz == 0) {
        cpuid(0x00000016, 0, regs);
        if (regs[1] > 0) info.frequencyMaxMHz = regs[1] & 0xFFFF;
    }
    
    return info;
}

IotaInfo::MemoryInfo IotaInfo::getMemoryInfo() {
    MemoryInfo info = {};
    MEMORYSTATUSEX ms = {};
    ms.dwLength = sizeof(ms);
    
    if (GlobalMemoryStatusEx(&ms)) {
        info.totalPhysicalMB = ms.ullTotalPhys / (1024 * 1024);
        info.availablePhysicalMB = ms.ullAvailPhys / (1024 * 1024);
        info.loadPercent = (double)ms.dwMemoryLoad;
        info.totalVirtualMB = ms.ullTotalPageFile / (1024 * 1024);
        info.availableVirtualMB = ms.ullAvailPageFile / (1024 * 1024);
    }
    
    info.frequencyMHz = 0;
    
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD speed = 0, size = sizeof(speed);
        RegQueryValueExA(hKey, "~MHz", nullptr, nullptr, (LPBYTE)&speed, &size);
        RegCloseKey(hKey);
    }

    DWORD smbiosSize = GetSystemFirmwareTable(0x52534D42, 0, nullptr, 0);
    if (smbiosSize > 0) {
        std::vector<BYTE> smbiosData(smbiosSize);
        if (GetSystemFirmwareTable(0x52534D42, 0, smbiosData.data(), smbiosSize) == smbiosSize) {
            BYTE* data = smbiosData.data() + 8;
            BYTE* end = smbiosData.data() + smbiosSize;
            
            while (data + 4 <= end) {
                if (data[0] == 17 && data[1] >= 21) {
                    uint16_t speed = *(uint16_t*)(data + 0x15);
                    if (speed > 0 && speed < 10000) {
                        info.frequencyMHz = speed;
                        break;
                    }
                }
                data += data[1];
                while (data + 1 < end && !(data[0] == 0 && data[1] == 0)) data++;
                data += 2;
            }
        }
    }
    
    return info;
}

static std::vector<IotaInfo::GPUInfo> g_gpuCache;
static bool g_gpuCached = false;

std::vector<IotaInfo::GPUInfo> IotaInfo::getGPUInfo() {
    if (g_gpuCached) return g_gpuCache;
    std::vector<GPUInfo> gpus;
    
    VkApplicationInfo ai = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    ai.apiVersion = VK_API_VERSION_1_3;
    VkInstanceCreateInfo ci = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &ai };
    VkInstance inst;
    if (vkCreateInstance(&ci, nullptr, &inst) != VK_SUCCESS) return gpus;
    
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(inst, &count, nullptr);
    if (!count) { vkDestroyInstance(inst, nullptr); return gpus; }
    
    std::vector<VkPhysicalDevice> devs(count);
    vkEnumeratePhysicalDevices(inst, &count, devs.data());
    
    for (auto dev : devs) {
        GPUInfo info = {};
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(dev, &props);
        
        info.name = props.deviceName;
        info.isDiscrete = (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
        
        switch (props.vendorID) {
            case 0x10DE: info.vendor = "NVIDIA"; break;
            case 0x1002: info.vendor = "AMD"; break;
            case 0x8086: info.vendor = "Intel"; break;
            default: info.vendor = "Unknown"; break;
        }
        
        VkPhysicalDeviceMemoryProperties mem;
        vkGetPhysicalDeviceMemoryProperties(dev, &mem);
        uint64_t vram = 0;
        for (uint32_t i = 0; i < mem.memoryHeapCount; i++)
            if (mem.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                vram += mem.memoryHeaps[i].size;
        info.vramTotalMB = vram / (1024 * 1024);
        
        gpus.push_back(info);
    }
    vkDestroyInstance(inst, nullptr);
    g_gpuCache = gpus;
    g_gpuCached = true;
    return gpus;
}

IotaInfo::GPUInfo IotaInfo::getPrimaryGPUInfo() {
    auto gpus = getGPUInfo();
    if (gpus.empty()) return GPUInfo{};
    
    for (auto& gpu : gpus) {
        if (gpu.isDiscrete) return gpu;
    }
    
    return gpus[0];
}

IotaInfo::OSInfo IotaInfo::getOSInfo() {
    OSInfo info = {};
    
    #ifdef _WIN64
    info.is64bit = true;
    #else
    info.is64bit = false;
    #endif
    
    OSVERSIONINFOEXA osvi;
    ZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    
    #pragma warning(push)
    #pragma warning(disable: 4996)
    if (GetVersionExA((LPOSVERSIONINFOA)&osvi)) {
    #pragma warning(pop)
        info.version = std::to_string(osvi.dwMajorVersion) + "." + std::to_string(osvi.dwMinorVersion) + "." + std::to_string(osvi.dwBuildNumber);
        
        if (osvi.dwMajorVersion == 10 && osvi.dwBuildNumber >= 22000) {
            info.name = "Windows 11";
        } else if (osvi.dwMajorVersion == 10) {
            info.name = "Windows 10";
        } else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
            info.name = "Windows 8.1";
        } else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
            info.name = "Windows 8";
        } else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
            info.name = "Windows 7";
        } else {
            info.name = "Windows";
        }
    }
    
    info.kernelVersion = info.version;
    info.uptimeSeconds = GetTickCount64() / 1000;
    
    return info;
}

IotaInfo::PerformanceTier IotaInfo::getPerformanceTier() {
    auto cpu = getCPUInfo();
    auto mem = getMemoryInfo();
    auto gpu = getPrimaryGPUInfo();
    int score = 0;
    
    if (cpu.numCoresPhysical >= 8) score += 3;
    else if (cpu.numCoresPhysical >= 6) score += 2;
    else if (cpu.numCoresPhysical >= 4) score += 1;
    
    if (cpu.frequencyMaxMHz >= 4000) score += 2;
    else if (cpu.frequencyMaxMHz >= 3000) score += 1;
    
    if (mem.totalPhysicalMB >= 32768) score += 3;
    else if (mem.totalPhysicalMB >= 16384) score += 2;
    else if (mem.totalPhysicalMB >= 8192) score += 1;
    
    if (gpu.vramTotalMB >= 16384) score += 4;
    else if (gpu.vramTotalMB >= 12288) score += 3;
    else if (gpu.vramTotalMB >= 8192) score += 2;
    else if (gpu.vramTotalMB >= 4096) score += 1;
    
    if (gpu.isDiscrete) score += 2;
    
    if (score >= 11) return PerformanceTier::ULTRA;
    if (score >= 8) return PerformanceTier::HIGH;
    if (score >= 5) return PerformanceTier::MEDIUM;
    return PerformanceTier::LOW;
}

IotaInfo::Settings IotaInfo::getSettings() {
    Settings settings = {};
    settings.tier = getPerformanceTier();
    
    switch (settings.tier) {
        case PerformanceTier::ULTRA:
            settings.enableRayTracing = true;
            settings.enableDLSS = false;
            settings.shadowQuality = 3;
            settings.textureQuality = 4;
            settings.antialiasing = 4;
            settings.resolutionScale = 100;
            settings.targetFPS = 144;
            settings.maxVRAMBudgetMB = 12288;
            settings.maxRAMBudgetMB = 16384;
            break;
            
        case PerformanceTier::HIGH:
            settings.enableRayTracing = true;
            settings.enableDLSS = true;
            settings.shadowQuality = 3;
            settings.textureQuality = 3;
            settings.antialiasing = 3;
            settings.resolutionScale = 100;
            settings.targetFPS = 60;
            settings.maxVRAMBudgetMB = 8192;
            settings.maxRAMBudgetMB = 8192;
            break;
            
        case PerformanceTier::MEDIUM:
            settings.enableRayTracing = false;
            settings.enableDLSS = true;
            settings.shadowQuality = 2;
            settings.textureQuality = 2;
            settings.antialiasing = 2;
            settings.resolutionScale = 85;
            settings.targetFPS = 60;
            settings.maxVRAMBudgetMB = 4096;
            settings.maxRAMBudgetMB = 4096;
            break;
            
        case PerformanceTier::LOW:
            settings.enableRayTracing = false;
            settings.enableDLSS = true;
            settings.shadowQuality = 1;
            settings.textureQuality = 1;
            settings.antialiasing = 1;
            settings.resolutionScale = 50;
            settings.targetFPS = 30;
            settings.maxVRAMBudgetMB = 2048;
            settings.maxRAMBudgetMB = 2048;
            break;
    }
    
    return settings;
}

#endif // PLATFORM_WINDOWS
#endif // #if defined (iota_WMI_IMPL)