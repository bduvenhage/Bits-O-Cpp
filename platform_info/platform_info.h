//=====================//
//=== Platform info ===//
//=====================//
#include <cpuid.h>
#include <x86intrin.h>

namespace platform_info {
    double get_cpu_sips() {
        const double spin_start_time = TCTimer::get_tsc_time();
        const int spin_count = 16777215;// max 16777215
        __m128 x = _mm_setzero_ps();
        for(int i=0; i<spin_count; i++) x = _mm_add_ps(x,_mm_set1_ps(1.0f));
        const int spin_count_result = _mm_cvt_ss2si(x);
        const double spin_end_time = TCTimer::get_tsc_time();
        const double sips = spin_count_result / ((spin_end_time - spin_start_time) * 1000000.0);
        return sips;
    }
    
    std::string get_cpu() {
        std::cerr << "get_cpu_info(): \n";
        char CPUBrandString[64];
        std::memset(CPUBrandString, 0, sizeof(CPUBrandString));
        uint32_t CPUInfo[4] = {0,0,0,0};
        __cpuid(0x80000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
        uint32_t nExIds = CPUInfo[0];
        
        for (uint32_t i = 0x80000000; i <= nExIds; ++i) {
            __cpuid(i, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
            
            if (i == 0x80000002) memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            else if (i == 0x80000003) memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
            else if (i == 0x80000004) memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
            
            if (i == 0x80000016) {
                std::cerr << "  Processor Base Frequency:  " << CPUInfo[0] << " MHz" << "\n";
                std::cerr << "  Maximum Frequency:         " << CPUInfo[1] << " MHz" << "\n";
                std::cerr << "  Bus (Reference) Frequency: " << CPUInfo[2] << " MHz" << "\n";
                std::cerr.flush();
            }
        }
        
        std::cerr << "  CPUBrandString: " << CPUBrandString << "\n";
        return CPUBrandString;
    }
    
    std::string get_compiler() {
        char compiler_info[1024];
#if defined(__clang__)
        sprintf(compiler_info, "clang %d.%d.%d", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__) || defined(__GNUG__)
        sprintf(compiler_info, "gcc %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
        sprintf(compiler_info, "Unknown compiler.");
#endif
        return compiler_info;
    }
}

