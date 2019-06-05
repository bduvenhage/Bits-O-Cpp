#define ALWAYS_INLINE inline __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))

#ifdef TCDEBUG
#define BBBD(a) if (a) {std::cerr << "BBBoomD " << #a << " @ " << __FILE__ << " line "<< __LINE__ << "!\n"; std::cerr.flush(); exit(-1);}
#else
#define BBBD(a)
#endif

#define DBS(a) std::cerr << #a << " = " << (a) << "  ";
#define DBN(a) std::cerr << #a << " = " << (a) << std::endl;

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

//! Test the performance of the testing code.
double test_baseline_perf(const uint64_t num_iterations) {
    double sum_sink = 0.0;
    const double start_time = TCTimer::get_time();

    for (uint64_t i=0; i<num_iterations; ++i) {
        const double r = rng.next_double();
        sum_sink += r;
    }
    
    const double end_time = TCTimer::get_time();
    DBN(sum_sink)
    return (end_time - start_time) / num_iterations;
}

//! Test the performance of the testing code with `exp`.
double test_exp_perf(const uint64_t num_iterations) {
    double sum_sink = 0.0;
    const double start_time = TCTimer::get_time();
    
    for (uint64_t i=0; i<num_iterations; ++i) {
        const double r = rng.next_double();
        sum_sink += exp(r);
    }
    
    const double end_time = TCTimer::get_time();
    DBN(sum_sink)
    return (end_time - start_time) / num_iterations;
}

//! Test the performance of the testing code with `fast_exp`.
double test_fast_exp_perf(const uint64_t num_iterations) {
    double sum_sink = 0.0;
    const double start_time = TCTimer::get_time();
    
    for (uint64_t i=0; i<num_iterations; ++i) {
        const double r = rng.next_double();
        sum_sink += tc_math::fast_exp_64(r);
    }
    
    const double end_time = TCTimer::get_time();
    DBN(sum_sink)
    return (end_time - start_time) / num_iterations;
}

//! Sigmoid using `exp`.
double sigmoid(const double x)
{
    return 1.0 / (1.0 + exp(-x));
}

//! Sigmoid using `fast_exp`.
double fast_sigmoid(const double x)
{
    return 1.0 / (1.0 + tc_math::fast_exp_64(-x));
}

//! Test the accuracy of `fast_exp` and optionally write a results.csv file.
double test_accuracy(const double x_l, const double x_r,
                     const uint64_t num_samples,
                     const std::string csv_filename = "") {
    const double dx = (x_r - x_l) / num_samples;
    uint64_t actual_num_samples = 0;
    double abs_error = 0.0;
    
    std::ofstream img_stream;
    
    if (csv_filename != "") {
        img_stream.open(csv_filename);
        img_stream << "x" << ", " << "exp" << ", " << "fast_exp" << ", " << "sigmoid" << ", " << "fast_sigmoid" << "\n";
        img_stream.precision(20);
    }
    
    for (double x=x_l; x<x_r; x+=dx) {
        const double e = exp(x);
        const double fe = tc_math::fast_exp_64(x);
        const double abs_sample_error = fabs(fe - e);
        
        if (img_stream.is_open())
        {
            const double s = sigmoid(x);
            const double fs = fast_sigmoid(x);
        
            img_stream << x << ", " << e << ", " << fe << ", " << s << ", " << fs << "\n";
        }
        
        abs_error += abs_sample_error;
        actual_num_samples += 1;
    }
    
    //Average abs_sample_error
    return abs_error / actual_num_samples;
}

int main(void)
{
    DBN(platform_info::get_cpu())
    DBN(platform_info::get_compiler())
    
    const double EXP_TSC_FREQ = 2.89992e+09; // Doesn't really matter when using get_time() aot get_tsc_time().
    TCTimer::init_timer(EXP_TSC_FREQ);
    DBN(TCTimer::get_seconds_per_tick())

    const uint64_t num_iterations = 300000000;

    const double baseline_perf = test_baseline_perf(num_iterations);
    const double exp_perf = test_exp_perf(num_iterations);
    const double fast_exp_perf = test_fast_exp_perf(num_iterations);

    std::cout << "\n";
    std::cout << "exp_perf = " << (exp_perf - baseline_perf) << " s/call \n";
    std::cout << "fast_exp_perf = " << (fast_exp_perf - baseline_perf) << " s/call \n";
    
    std::cout << "\n";
    std::cout << "Average abs_sample_error = " << test_accuracy(-5.5, 5.5, 1000) << "\n";

    std::cout.flush();

    test_accuracy(-5.5, 5.5, 1000, "results_global_fit.csv");
    test_accuracy(3.152, 3.856, 1000, "results_lin_interp.csv");
    test_accuracy(3.46573, 3.46574, 1000, "results_staircase.csv");
    
    return 0;
}
