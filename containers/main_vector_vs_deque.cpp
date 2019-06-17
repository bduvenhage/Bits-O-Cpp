// Compare the performance of deque to vector.

#include "../defines/tc_defines.h"

#include "../time/tc_timer.h"
#include "../random/tc_random_funcs.h"
#include "../platform_info/platform_info.h"

#include <deque>
#include <vector>
#include <algorithm>
#include <iostream>


TCRandom<TC_MCG_Lehmer_RandFunc32> rng(987654321);
std::deque<int> d;
std::vector<int> v;

double vector_push_back = 0.0;
double vector_push_front = 0.0;
double vector_sort = 0.0;
double vector_iterate = 0.0;
double vector_pop_back = 0.0;

double deque_push_back = 0.0;
double deque_push_front = 0.0;
double deque_sort = 0.0;
double deque_iterate = 0.0;
double deque_pop_back = 0.0;


void test_vector(const int num_iterations)
{
    //v.reserve(num_iterations + 2); //Ensure there is enough space reserved.
    
    // == PUSH_BACK
    {
        double start_time = TCTimer::get_time();
        
        for (int i=0; i<(num_iterations>>1); ++i)
        {
            v.push_back(rng.next());
        }
        
        double end_time = TCTimer::get_time();
        vector_push_back = end_time - start_time;
    }
    
    // == INSERT FRONT
    {
        double start_time = TCTimer::get_time();
        
        for (int i=0; i<(num_iterations>>1); ++i)
        {
            //Takes approx. an infinite amount of time. DON'T try this!
            //v.insert(v.begin(), rng.next());
            
            // Rather add some more elements to back.
            v.push_back(rng.next());
        }
        
        double end_time = TCTimer::get_time();
        vector_push_front = end_time - start_time;
    }
    
    // == SORT
    {
        double start_time = TCTimer::get_time();
        
        std::sort(v.begin(), v.end());
        
        double end_time = TCTimer::get_time();
        vector_sort = end_time - start_time;
    }
    
    // == ITERATE
    {
        double start_time = TCTimer::get_time();
        
        for (int i=0; i<num_iterations; ++i)
        {
            v[i] = 2;
        }
        
        double end_time = TCTimer::get_time();
        vector_iterate = end_time - start_time;
    }
    
    // == POP BACK
    {
        double start_time = TCTimer::get_time();
        int number_sink = 0;
        
        for (int i=0; i<(num_iterations>>1); ++i)
        {
            number_sink += v.back();
            v.pop_back();
        }
        
        DBN(number_sink)
        double end_time = TCTimer::get_time();
        vector_pop_back = end_time - start_time;
    }
}

void test_deque(const int num_iterations)
{
    // == PUSH_BACK
    {
        double start_time = TCTimer::get_time();
        
        for (int i=0; i<(num_iterations>>1); ++i)
        {
            d.push_back(rng.next());
        }
        
        double end_time = TCTimer::get_time();
        deque_push_back = end_time - start_time;
    }
    
    // == INSERT FRONT
    {
        double start_time = TCTimer::get_time();
        
        for (int i=0; i<(num_iterations>>1); ++i)
        {
            d.push_front(rng.next());
        }
        
        double end_time = TCTimer::get_time();
        deque_push_front = end_time - start_time;
    }
    
    // == SORT
    {
        double start_time = TCTimer::get_time();
        
        std::sort(d.begin(), d.end());
        
        double end_time = TCTimer::get_time();
        deque_sort = end_time - start_time;
    }
    
    // == ITERATE
    {
        double start_time = TCTimer::get_time();
        
        for (int i=0; i<num_iterations; ++i)
        {
            d[i] = 2;
        }
        
        double end_time = TCTimer::get_time();
        deque_iterate = end_time - start_time;
    }
    
    // == POP BACK
    {
        double start_time = TCTimer::get_time();
        int number_sink = 0;
        
        for (int i=0; i<(num_iterations>>1); ++i)
        {
            number_sink += d.back();
            d.pop_back();
        }
        
        DBN(number_sink)
        double end_time = TCTimer::get_time();
        deque_pop_back = end_time - start_time;
    }
}


int main(void)
{
    DBN(platform_info::get_cpu())
    DBN(platform_info::get_compiler())

    const double EXP_TSC_FREQ = 2.89992e+09;
    TCTimer::init_timer(EXP_TSC_FREQ);
 
    int num_iterations = 100000000; // 100M elements * 4 bytes.
    num_iterations = (num_iterations >> 1) << 1; // Make sure num_iterations is even.

    std::cout << "Doing tests...";
    std::cout.flush();

    //Note: Only do one test at a time! Either test_vector OR test_deque.
    test_vector(num_iterations);
    //test_deque(num_iterations);
    
    std::cout << "done.\n";

    DBN(vector_push_back)
    //DBN(vector_push_front)
    DBN(vector_sort)
    DBN(vector_iterate)
    DBN(vector_pop_back)

    DBN(deque_push_back)
    DBN(deque_push_front)
    DBN(deque_sort)
    DBN(deque_iterate)
    DBN(deque_pop_back)

    return 0;
}
