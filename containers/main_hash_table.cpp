// Analyse the heap allocations done by C++ unordered_set. unordered_map should behave similarly.

#define ALWAYS_INLINE inline __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))

#ifdef TCDEBUG
#define BBBD(a) if (a) {std::cerr << "BBBoomD " << #a << " @ " << __FILE__ << " line "<< __LINE__ << "!\n"; std::cerr.flush(); exit(-1);}
#else
#define BBBD(a)
#endif

#define DBS(a) std::cerr << #a << " = " << (a) << "  ";
#define DBN(a) std::cerr << #a << " = " << (a) << std::endl;

#include "../time/tc_timer.h"
#include "../random/tc_random_funcs.h"
#include "../platform_info/platform_info.h"

#include <map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>


uint64_t tracked_alloc_total = 0; // Tracked total number of bytes currently allocated.
std::map<size_t, size_t> tracked_alloc_breakdown; // The total number of elements allocated by size of the type allocated.

// Simple minimal custom allocator to track total bytes and breakdown of allocations.
template<typename _Ty>
struct TrackingAllocator
{
    typedef _Ty value_type;
    static _Ty* allocate(std::size_t n)
    {
        //Code that runs every allocation
        tracked_alloc_total += sizeof(_Ty) * n;
        tracked_alloc_breakdown[sizeof(_Ty)] = tracked_alloc_breakdown[sizeof(_Ty)] + n;
        return std::allocator<_Ty>{}.allocate(n);
    }
    
    static void deallocate(_Ty* mem, std::size_t n)
    {
        //Code that runs every deallocation
        tracked_alloc_total -= sizeof(_Ty) * n;
        tracked_alloc_breakdown[sizeof(_Ty)] = tracked_alloc_breakdown[sizeof(_Ty)] - n;
        std::allocator<_Ty>{}.deallocate(mem, n);
    }
};

TCRandom<TC_MCG_Lehmer_RandFunc32> rng(987654321); // Fast random number generator used to fill set.

std::unordered_set<uint32_t,
                   std::hash<uint32_t>,
                   std::equal_to<uint32_t>,
                   TrackingAllocator<uint32_t>> s;


// valgrind --tool=massif prog

int main(void)
{
    DBN(platform_info::get_cpu())
    DBN(platform_info::get_compiler())

    const double EXP_TSC_FREQ = 2.89992e+09;
    TCTimer::init_timer(EXP_TSC_FREQ);
 
    int num_iterations = 20000000;
    
    std::ofstream fout("out.csv");

    fout << "s.size()" << ", ";
    fout << "TCTimer::get_time()" << ", ";
    fout << "s.load_factor()" << ", ";
    fout << "s.bucket_count()" << "\n";
    fout.flush();

    DBN(sizeof(s))
    DBN(s.max_load_factor())

    std::cout << "Doing tests...";
    std::cout.flush();
    
    double start_time = TCTimer::get_time();

    for (int i=0; i<num_iterations; ++i)
    {
        s.insert(rng.next());
        
        if ((i % 100000) == 0)
        {
            DBS(TCTimer::get_time())
            DBS(s.load_factor())
            DBS(s.bucket_count())
            DBN(tracked_alloc_total/(1024*1024))

            for (auto bd : tracked_alloc_breakdown)
            {
                const auto type_size = bd.first;
                const auto type_count = bd.second;
                
                DBS(type_size)
                DBN(type_count)
            }
            
            DBN(s.size())
            std::cout << "\n";

            fout << s.size() << ", ";
            fout << TCTimer::get_time() << ", ";
            fout << s.load_factor() << ", ";
            fout << s.bucket_count() << "\n";
            fout.flush();
        }
    }
    
    double end_time = TCTimer::get_time();

    std::cout << "done.\n";
    
    DBN(sizeof(s))
    DBN(end_time - start_time)
    
    fout.close();

    return 0;
}
