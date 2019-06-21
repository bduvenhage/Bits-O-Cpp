// Analyse the heap allocations done by C++ unordered_set. unordered_map should behave similarly.

#include "../defines/tc_defines.h"

#include "../time/tc_timer.h"
#include "../random/tc_random_funcs.h"
#include "../platform_info/platform_info.h"

#include <map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>


uint64_t tracked_allocator_total = 0; // Tracked total number of bytes currently allocated.
std::map<size_t, int64_t> tracked_allocator_breakdown_alloc; // The total number of items allocated by size of type.
std::map<size_t, int64_t> tracked_allocator_breakdown_dealloc; // The total number of items de-allocated by size of type.

// Simple minimal custom allocator to track total bytes and breakdown of allocations.
template<typename _Ty>
struct TrackingAllocator
{
    typedef _Ty value_type;
    static _Ty* allocate(std::size_t n)
    {
        //Code that runs every allocation
        tracked_allocator_total += sizeof(_Ty) * n;
        tracked_allocator_breakdown_alloc[sizeof(_Ty)] = tracked_allocator_breakdown_alloc[sizeof(_Ty)] + n;
        return std::allocator<_Ty>{}.allocate(n);
    }
    
    static void deallocate(_Ty* mem, std::size_t n)
    {
        //Code that runs every deallocation
        tracked_allocator_total -= sizeof(_Ty) * n;
        tracked_allocator_breakdown_dealloc[sizeof(_Ty)] = tracked_allocator_breakdown_dealloc[sizeof(_Ty)] + n;
        std::allocator<_Ty>{}.deallocate(mem, n);
    }
};

std::unordered_set<uint32_t,
                   std::hash<uint32_t>,
                   std::equal_to<uint32_t>,
                   TrackingAllocator<uint32_t>> s;

struct BucketItem
{
    size_t hash_;
    uint32_t item_;
    BucketItem *next_;
};

int main(void)
{
    DBN(platform_info::get_cpu_brand_string())
    DBN(platform_info::get_compiler())

    const double EXP_TSC_FREQ = 2.89992e+09;
    TCTimer::init_timer(EXP_TSC_FREQ);
        TCRandom<TC_MCG_Lehmer_RandFunc32> rng(987654321); // Fast random number generator used to fill set.
    
    int num_iterations = 20000000;
    
    std::ofstream fout("out.csv");

    fout << "s.size()" << ", ";
    fout << "TCTimer::get_time()" << ", ";
    fout << "tracked_allocator_total (MB)" << ", ";
    fout << "s.load_factor()" << ", ";
    fout << "s.bucket_count()" << "\n";
    fout.flush();

    DBN(sizeof(BucketItem))
    DBN(sizeof(s))
    DBN(s.max_load_factor())

    std::cout << "Doing tests...";
    std::cout.flush();
    
    double start_time = TCTimer::get_time();

    for (int i=0; i<num_iterations; ++i)
    {
        s.insert(rng.next());
        
        if (((i % 100000) == 0) ||
            (i == (num_iterations-1)) )
        {
            DBS(TCTimer::get_time())
            DBS(s.load_factor())
            DBS(s.bucket_count())
            DBN(tracked_allocator_total/(1024*1024))

            for (auto bd : tracked_allocator_breakdown_alloc)
            {
                const auto type_size = bd.first; // The size of the allocated item.
                
                const auto number_alloc = bd.second; // The number of item allocations (of this size).
                const auto number_dealloc = tracked_allocator_breakdown_dealloc[type_size];
                const auto number_on_heap = number_alloc - number_dealloc; // Number of items of this type size that is currently on heap!

                DBS(type_size)
                DBS(number_alloc)
                DBN(number_on_heap)
            }
            
            DBN(s.size())
            std::cout << "\n";

            fout << s.size() << ", ";
            fout << TCTimer::get_time() << ", ";
            fout << tracked_allocator_total/(1024*1024) << ", ";
            fout << s.load_factor() << ", ";
            fout << s.bucket_count() << "\n";
            fout.flush();
        }
    }
    
    double end_time = TCTimer::get_time();

    std::cout << "done.\n";
    
    DBN(sizeof(s))
    DBN(s.size())
    DBN(end_time - start_time)
    
    fout.close();

    return 0;
}
