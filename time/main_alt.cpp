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
    // DBHEX
    // DBN(platform_info::get_cpu_display_family())
    // DBN(platform_info::get_cpu_display_model())
    DBDEC
    DBN(platform_info::is_tsc_invariant())
    // DBN(platform_info::get_compiler())
    
    DBN(std::chrono::high_resolution_clock::is_steady)
    
    // === Time the timer ===//
    double iteration_time_sum = 0.0;
    const int num_iterations = 300000000;
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

    //// If the below compiles then high_resolution_timer is a fast realtime system_clock,
    //// else it is the steady_clock because it is faster on your system.
    // const time_t tt = std::chrono::high_resolution_clock::to_time_t(tp_end);
    
    DBN(time_per_iteration__seconds)
    DBN(time_per_iteration__cycles)
    DBN(seconds_per_tick)
    DBN(1.0/seconds_per_tick)
    DBN(sink)
}
