#if defined (iota_WMI_IMPL)

#include "../iotaWmi.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <vulkan/vulkan.h>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_UNIX)

IotaInfo::CPUInfo IotaInfo::getCPUInfo() {
    CPUInfo info = {};
    
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo.is_open()) return info;
    
    std::string line;
    int physicalCores = 0;
    
    while (std::getline(cpuinfo, line)) {
        if (line.find("vendor_id") != std::string::npos) {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) info.vendor = line.substr(pos + 2);
        }
        else if (line.find("model name") != std::string::npos) {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) info.brand = line.substr(pos + 2);
        }
        else if (line.find("cpu cores") != std::string::npos) {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) {
                physicalCores = std::stoi(line.substr(pos + 2));
            }
        }
        else if (line.find("cache size") != std::string::npos) {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) {
                std::string cacheStr = line.substr(pos + 2);
                info.cacheL3Size = std::stoi(cacheStr);
            }
        }
    }
    cpuinfo.close();
    
    info.numCoresLogical = sysconf(_SC_NPROCESSORS_ONLN);
    info.numCoresPhysical = physicalCores > 0 ? physicalCores : info.numCoresLogical;
    
    info.cacheLineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    if (info.cacheLineSize == 0) info.cacheLineSize = 64;
    
    long l1d = sysconf(_SC_LEVEL1_DCACHE_SIZE);
    long l1i = sysconf(_SC_LEVEL1_ICACHE_SIZE);
    if (l1d > 0 && l1i > 0) info.cacheL1Size = (l1d + l1i) * info.numCoresPhysical / 1024;
    
    long l2 = sysconf(_SC_LEVEL2_CACHE_SIZE);
    if (l2 > 0) info.cacheL2Size = l2 * info.numCoresPhysical / 1024;

    if (info.cacheL3Size == 0) {
        long l3 = sysconf(_SC_LEVEL3_CACHE_SIZE);
        if (l3 > 0) info.cacheL3Size = l3 / 1024;
    }

    std::ifstream freqFile("/sys/devices/system/cpu/cpu0/cpufreq/base_frequency");
    if (freqFile.is_open()) {
        uint64_t freq;
        freqFile >> freq;
        info.frequencyMHz = freq / 1000.0;
        freqFile.close();
    }
    
    if (info.frequencyMHz == 0) {
        std::ifstream mhzFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
        if (mhzFile.is_open()) {
            uint64_t freq;
            mhzFile >> freq;
            info.frequencyMaxMHz = freq / 1000.0;
            if (info.frequencyMHz == 0) info.frequencyMHz = info.frequencyMaxMHz;
            mhzFile.close();
        }
    }
    
    if (info.frequencyMHz == 0) {
        std::ifstream mhzFile("/proc/cpuinfo");
        if (mhzFile.is_open()) {
            std::string l;
            while (std::getline(mhzFile, l)) {
                if (l.find("cpu MHz") != std::string::npos) {
                    size_t pos = l.find(": ");
                    if (pos != std::string::npos) {
                        info.frequencyMHz = std::stod(l.substr(pos + 2));
                        break;
                    }
                }
            }
            mhzFile.close();
        }
    }
    
    return info;
}

IotaInfo::MemoryInfo IotaInfo::getMemoryInfo() {
    MemoryInfo info = {};
    
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        info.totalPhysicalMB = si.totalram / (1024 * 1024);
        info.availablePhysicalMB = si.freeram / (1024 * 1024);
        info.totalVirtualMB = (si.totalram + si.totalswap) / (1024 * 1024);
        info.availableVirtualMB = (si.freeram + si.freeswap) / (1024 * 1024);
        if (info.totalPhysicalMB > 0) {
            info.loadPercent = 100.0 * (1.0 - (double)si.freeram / si.totalram);
        }
        info.frequencyMHz = 0;
    }
    
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal") != std::string::npos) {
                uint64_t kb;
                sscanf(line.c_str(), "MemTotal: %lu", &kb);
                info.totalPhysicalMB = kb / 1024;
            }
            else if (line.find("MemAvailable") != std::string::npos) {
                uint64_t kb;
                sscanf(line.c_str(), "MemAvailable: %lu", &kb);
                info.availablePhysicalMB = kb / 1024;
            }
        }
        meminfo.close();
    }

    std::ifstream dmi("/sys/class/dmi/id/memory_device_speed");
    if (dmi.is_open()) {
        std::string speed;
        dmi >> speed;
        if (!speed.empty() && speed != "Unknown") {
            info.frequencyMHz = std::stoi(speed);
        }
        dmi.close();
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
    if (vkCreateInstance(&ci, nullptr, &inst) != VK_SUCCESS) {
        for (int i = 0; i < 10; i++) {
            std::string path = "/sys/class/drm/card" + std::to_string(i) + "/device/vendor";
            std::ifstream vfile(path);
            if (!vfile.is_open()) break;
            
            GPUInfo info = {};
            unsigned vendorId;
            vfile >> std::hex >> vendorId;
            vfile.close();
            
            switch (vendorId) {
                case 0x10de: info.vendor = "NVIDIA"; break;
                case 0x1002: info.vendor = "AMD"; break;
                case 0x8086: info.vendor = "Intel"; break;
                default: info.vendor = "Unknown";
            }
            
            info.name = info.vendor + " GPU " + std::to_string(i);
            info.vramTotalMB = 4096;
            info.isDiscrete = (info.vendor == "NVIDIA" || info.vendor == "AMD");
            
            gpus.push_back(info);
        }
        
        g_gpuCache = gpus;
        g_gpuCached = true;
        return gpus;
    }
    
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
    for (auto& g : gpus) if (g.isDiscrete) return g;
    return gpus.empty() ? GPUInfo{} : gpus[0];
}

IotaInfo::OSInfo IotaInfo::getOSInfo() {
    OSInfo info = {};
    
    struct utsname uts;
    if (uname(&uts) == 0) {
        info.name = uts.sysname;
        info.version = uts.release;
        info.kernelVersion = uts.version;
        info.is64bit = (std::string(uts.machine).find("64") != std::string::npos);
    }
    
    std::ifstream osRelease("/etc/os-release");
    if (osRelease.is_open()) {
        std::string line;
        while (std::getline(osRelease, line)) {
            if (line.find("PRETTY_NAME") != std::string::npos) {
                size_t start = line.find('"');
                size_t end = line.rfind('"');
                if (start != std::string::npos && end != std::string::npos && end > start) {
                    info.name = line.substr(start + 1, end - start - 1);
                }
            }
        }
        osRelease.close();
    }
    
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        info.uptimeSeconds = si.uptime;
    }
    
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

#endif // PLATFORM_LINUX || PLATFORM_UNIX
#endif // #if defined (iota_WMI_IMPL)