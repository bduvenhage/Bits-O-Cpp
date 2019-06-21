#include "../defines/tc_defines.h"

#include "../math/tc_math.h"
#include "../time/tc_timer.h"
#include "../random/tc_random_funcs.h"
#include "../platform_info/platform_info.h"

#include <map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>

TCRandom<TC_MCG_Lehmer_RandFunc32> rng(987654321); // Generally good fast generator.

double test_baseline_perf(const uint64_t num_iterations) {
    uint64_t sum_sink = 0.0;
    const double start_time = TCTimer::get_time();

    for (uint64_t i=0; i<num_iterations; ++i) {
        const uint32_t r = rng.next();
        sum_sink += r;
    }
    
    const double end_time = TCTimer::get_time();
    DBN(sum_sink)
    return (end_time - start_time) / num_iterations;
}

double test_intlog2_perf(const uint64_t num_iterations) {
    int sum_sink = 0.0;
    const double start_time = TCTimer::get_time();
    
    for (uint64_t i=0; i<num_iterations; ++i) {
        const uint32_t r = rng.next();
        sum_sink += int(log2(double(r)));
    }
    
    const double end_time = TCTimer::get_time();
    DBN(sum_sink)
    return (end_time - start_time) / num_iterations;
}

double test_fast_intlog2_perf(const uint64_t num_iterations) {
    int sum_sink = 0.0;
    const double start_time = TCTimer::get_time();
    
    for (uint64_t i=0; i<num_iterations; ++i) {
        const uint32_t r = rng.next();
        sum_sink += tc_math::fast_int_log2(r);
    }
    
    const double end_time = TCTimer::get_time();
    DBN(sum_sink)
    return (end_time - start_time) / num_iterations;
}

double test_accuracy(const int x_l, const int x_r) {
    uint64_t actual_num_samples = 0;
    double abs_error = 0.0;
    
    for (int x=x_l; x<x_r; ++x) {
        const int il2 = int(log2(double(x)));
        const int fil2 = tc_math::fast_int_log2(x);
        const double abs_sample_error = fabs(fil2 - il2);
        
        abs_error += abs_sample_error;
        actual_num_samples += 1;
    }
    
    //Average abs_sample_error
    return abs_error / actual_num_samples;
}

int main(void)
{
    DBN(platform_info::get_cpu_brand_string())
    DBN(platform_info::get_compiler())
    
    const double EXP_TSC_FREQ = 2.89992e+09; // Doesn't really matter when using get_time() aot get_tsc_time().
    TCTimer::init_timer(EXP_TSC_FREQ);

    const uint64_t num_iterations = 300000000;

    const double baseline_perf = test_baseline_perf(num_iterations);
    const double intlog2_perf = test_intlog2_perf(num_iterations);
    const double fast_intlog2_perf = test_fast_intlog2_perf(num_iterations);

    std::cout << "\n";
    std::cout << "intlog2_perf = " << (intlog2_perf - baseline_perf) << " s/call \n";
    std::cout << "fast_intlog2_perf = " << (fast_intlog2_perf - baseline_perf) << " s/call \n";
    
    std::cout << "\n";
    std::cout << "Average abs_sample_error = " << test_accuracy(1, 1000000) << "\n";
    
    std::cout.flush();

    return 0;
}
