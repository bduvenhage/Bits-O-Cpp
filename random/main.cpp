#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define TCDEBUG 1

#ifdef TCDEBUG
#define BBBD(a) if (a) {std::cerr << "BBBoomD " << #a << " @ " << __FILE__ << " line "<< __LINE__ << "!\n"; std::cerr.flush(); exit(-1);}
#else
#define BBBD(a)
#endif

//std::cerr << std::setprecision(10); ???
#define DBS(a) std::cerr << #a << " = " << (a) << "  ";
#define DBN(a) std::cerr << #a << " = " << (a) << std::endl;

#define ALWAYS_INLINE inline __attribute__((always_inline))

#include "../time/tc_timer.h"
#include "tc_random_funcs.h"
#include "../platform_info/platform_info.h"


// ToDo: what would be a realistic test case that would realistically flush the cache and registers?


int main()
{
    const uint32_t rng_seed_ = 0;
    TCRandom<TC_MCG_Lehmer_RandFunc32> lehmer_rng(rng_seed_);
    
    DBN(platform_info::is_intel_cpu())
    DBN(platform_info::is_drng_supported())
    TCRandom<TC_IntelDRNG_RandFunc32> intel_rng_(rng_seed_);

    std::cout << "Generating some random numbers...";
    
    TCTimer::init_timer(2.89992e+09);
    
    const uint64_t num_iterations = uint64_t(1) << 27;
    uint32_t ri = 0;
    
    const double start_time = TCTimer::get_time();

    for (uint64_t i=0; i <= num_iterations; ++i)
    {
        //ri += lehmer_rng.next();
        ri += intel_rng_.next();
        //ri += rdseed64();
    }
    
    const double end_time = TCTimer::sync_tsc_time(); // Same as get_time(), but also estimates CPU's seconds_per_tick_.
    
    std::cout << "done.\n";
    
    const double cpu_ticks_per_ns = TCTimer::get_clock_freq() * 0.000000001;
    const double ns_per_iteration = (end_time-start_time) / num_iterations * 1000000000.0;
    const double cpu_ticks_per_iteration = cpu_ticks_per_ns * ns_per_iteration;
    
    const double millions_numbers_per_second = num_iterations / (end_time-start_time) * 0.000001;
    
    DBN(cpu_ticks_per_ns)
    DBN(ns_per_iteration)
    DBN(cpu_ticks_per_iteration)
    
    DBN(millions_numbers_per_second)
    
    DBN(ri) // Print the sum so that the RNG doesn't get optimised out.

    return 0;
}
