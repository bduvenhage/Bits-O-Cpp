//! Example - std::chrono::high_resolution_clock

#include "../defines/tc_defines.h"

#include "../math/tc_math.h"
#include "../random/tc_random_funcs.h"
#include "../platform_info/platform_info.h"

#include <array>
#include <thread>
#include <ctime>
#include <chrono>


TCRandom<TC_MCG_Lehmer_RandFunc32> rng_;

int main(void)
{
    DBN(platform_info::get_cpu_brand_string())
    DBHEX
    DBS(platform_info::get_cpu_display_family())
    DBN(platform_info::get_cpu_display_model())
    DBDEC
    DBN(platform_info::is_tsc_invariant())
    DBN(platform_info::get_TSC_freq())
    DBN(std::chrono::high_resolution_clock::is_steady)
    DBN(platform_info::get_compiler())
    
    std::cout << "\n";
    std::cout << "Compare chrono::steady_clock and clock_gettime's CLOCK_UPTIME_RAW:\n";
    // std::cout << "On my laptop high_resolution_clock was steady_clock and the same as CLOCK_UPTIME_RAW!\n";
    // std::cout << "[Intel(R) Core(TM) i5-5287U CPU @ 2.90GHz with Apple LLVM (clang) 10.0.1.]\n";
    timespec clock_getres_tp;
    clock_getres(CLOCK_UPTIME_RAW, &clock_getres_tp);
    DBN(clock_getres_tp.tv_nsec)

    timespec clock_gettime_tp;
    clock_gettime(CLOCK_UPTIME_RAW, &clock_gettime_tp);
    auto chrono_steady_now = std::chrono::steady_clock::now();

    DBS(clock_gettime_tp.tv_sec)
    DBN(clock_gettime_tp.tv_nsec)
    
    const uint64_t chrono_steady_nsec_raw = std::chrono::time_point_cast<std::chrono::nanoseconds>(chrono_steady_now).time_since_epoch().count();
    const uint64_t chrono_steady_sec = chrono_steady_nsec_raw / uint64_t(1000000000);
    const uint64_t chrono_steady_nsec = chrono_steady_nsec_raw % uint64_t(1000000000);
    DBS(chrono_steady_sec)
    DBN(chrono_steady_nsec)
    std::cout << "\n";

    // === Time the std::chrono timer ===//
    double iteration_time_sum = 0.0;
    const int num_iterations = 100000000;
    double sink = 0.0; // bit sink to prevent things from being compiled out.

    std::cout << "\nTesting the timer ...";
    std::cout.flush();
    
    std::chrono::high_resolution_clock::time_point tp_start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point tp_0 = tp_start;

    for (int j=0; j<num_iterations; ++j)
    {
        // // Do some random work.
        // for (int i=0; i<0; ++i)
        //     sink += rng_.next();
        
        // Time the timer.
        // - Note: You shouldn't time in inner loop if you can avoid it, but done here to measure performance of timer.
        const std::chrono::high_resolution_clock::time_point tp_1 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(tp_1 - tp_0);
        
        iteration_time_sum += time_span.count();
        tp_0 = tp_1;
    }
    
    std::chrono::high_resolution_clock::time_point tp_end = std::chrono::high_resolution_clock::now();
    std::cout << "done.\n\n";

    DBN(iteration_time_sum / num_iterations)

    const std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(tp_end - tp_start);
    const double seconds_per_tick = std::chrono::high_resolution_clock::period::num / double(std::chrono::high_resolution_clock::period::den);
    
    const double time_per_iteration__seconds = time_span.count() / num_iterations;
    const double time_per_iteration__cycles = (time_span.count() / num_iterations) / seconds_per_tick;

    //// If the below to_time_t compiles then high_resolution_timer is a fast realtime system_clock,
    //// else it is the steady_clock because it is faster on your system.
    // const time_t tt = std::chrono::high_resolution_clock::to_time_t(tp_end);
    
    DBN(time_per_iteration__seconds)
    DBN(time_per_iteration__cycles)
    DBN(seconds_per_tick)
    DBN(1.0/seconds_per_tick)
    DBN(sink)
}
