#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//#include <sys/time.h>

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


// ToDo: what would be a realistic test case that would realistically flush the cache and registers?


int main()
{
    const uint32_t rng_seed_ = 0;
    TCRandom<TC_MCG_Lehmer_RandFunc32> lehmer_rng(rng_seed_);
    TCRandom<TC_IntelDRNG_RandFunc32> intel_rng_(rng_seed_);

    TCTimer::init_timer(2.89992e+09);
    
    const double start_time = TCTimer::get_tsc_time();
    
    const uint64_t num_iterations = uint64_t(1) << 33;
    uint32_t ri = 0;
    
    for (uint64_t i=0; i <= num_iterations; ++i)
    {
        ri += lehmer_rng.next();
    }
    
    //const double end_time = TCTimer::get_tsc_time();
    const double end_time = TCTimer::sync_tsc_time();
    
    DBN(TCTimer::get_seconds_per_tick() * 1000000000.0)
    DBN((end_time-start_time) * 1000000000.0 / num_iterations)
    DBN(ri)

    return 0;
}
