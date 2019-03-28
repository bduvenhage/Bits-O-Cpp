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
    //TCRandom<TC_MCG_Lehmer_RandFunc32> rng_(rng_seed_);
    TCRandom<TC_IntelDRNG_RandFunc32> rng_(rng_seed_);

    TCTimer::init_timer(2.89992e+09);
    
    const double start_time = TCTimer::get_tsc_time();
    
    const uint64_t num_iterations = uint64_t(1) << 25;
    //uint64_t t=0,f=0;
    uint32_t ri = 0;
    //double rf = 0.0;
    
    for (uint64_t i=0; i <= num_iterations; ++i)
    {
        ri += rng_.next();
        //ri = rng_.next();
        //rf += rng_.next_double();
        //t+=ri&1;
        //f+=!(ri&1);
    }
    
    //const double end_time = TCTimer::get_tsc_time();
    const double end_time = TCTimer::sync_tsc_time();
    
    DBN(TCTimer::get_clock_freq())
    
    DBN(ri)
    //DBN(rf/num_iterations)
    //DBN(t/double(f))
    DBN((end_time-start_time) * 1000000000.0 / num_iterations)
    
    return 0;
}
