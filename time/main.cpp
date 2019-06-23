#include "../defines/tc_defines.h"

#include "../time/tc_timer.h"
#include "../math/tc_math.h"
#include "../random/tc_random_funcs.h"
#include "../platform_info/platform_info.h"

#include <array>
#include <thread>
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
    DBN(platform_info::get_TSC_freq())
    // DBN(platform_info::get_compiler())
    
    // === Init timer ===//
    //1) Init the timer with a guess of the CPUs freq in Hz. It will update later and need not be super accurate now.
    TCTimer::init_timer(2800000000);
    
    //2) Do some other things or sleep a bit to have some TSC and wall clock ticks pass.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    //3) Use the TSC and wall clock ticks since inits to calculate an accurate CPU freq.
    //   * From now get_tsc_time() is accurate!
    TCTimer::sync_tsc_time(); //
    DBN(TCTimer::get_clock_freq())
    // DBN(TCTimer::get_tsc_time())
    
    // === Time the timer ===//
    double iteration_time_sum = 0.0;
    const int num_iterations = 300000000;
    double sink = 0.0; // bit sink to prevent things from being compiled out.

    std::cout << "\nTesting the timer ...";
    std::cout.flush();
    
    const double start_time = TCTimer::get_tsc_time();
    double t0 = start_time;

    for (int j=0; j<num_iterations; ++j)
    {
        // // Do some random work.
        // for (int i=0; i<0; ++i)
        //     sink += rng_.next();
        
        // Time the timer.
        // - Note: You shouldn't time in inner loop if you can avoid it, but done here to measure performance of timer.
        const double t1 = TCTimer::get_tsc_time();
        iteration_time_sum += t1 - t0;
        t0 = t1;
    }
    
    const double end_time = TCTimer::get_tsc_time();
    std::cout << "done.\n\n";

    DBN(iteration_time_sum / num_iterations)

    const double time_per_iteration__seconds = (end_time - start_time) / num_iterations;
    const double time_per_iteration__cycles = ((end_time - start_time) / num_iterations) / TCTimer::get_seconds_per_tick();

    DBN(time_per_iteration__seconds)
    DBN(time_per_iteration__cycles)
    DBN(sink)
}
