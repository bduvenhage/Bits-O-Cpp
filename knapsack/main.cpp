#include <iostream>
#include <vector>

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


//! Solve the knapsack problem using a greedy algorithm.
std::vector<int> knapsack_greedy(const std::vector<std::pair<int, int>> &weight_value_vect_,
                                 const int W_) {
    const int weight_value_vect_size = weight_value_vect_.size();
    std::vector<bool> object_used(weight_value_vect_size, false);
    int num_objects_used = 0;

    // int64_t knapsack_value = 0;
    int solution_weight = 0;
    bool busy;
    
    do {
        busy = false;
        int best_value_per_weight_index = -1;
        double best_value_per_weight = 0;
        
        for (int i=0; i<weight_value_vect_size; ++i) {
            if ((object_used[i] == false) &&
                ((solution_weight + weight_value_vect_[i].first) <= W_))
            {// Unused object that still fits in knapsack.
                const double value_per_weight = weight_value_vect_[i].second / double(weight_value_vect_[i].first);
                
                if (value_per_weight > best_value_per_weight)
                {// Better value_per_weight found.
                    best_value_per_weight = value_per_weight;
                    best_value_per_weight_index = i;
                }
            }
        }
        
        if (best_value_per_weight_index != -1) {
            busy = true;
            
            object_used[best_value_per_weight_index] = true;
            num_objects_used += 1;
            
            // knapsack_value += weight_value_vect_[best_value_per_weight_index].second;
            solution_weight += weight_value_vect_[best_value_per_weight_index].first;
        }
    } while (busy);
    
    // Output
    std::vector<int> object_ids;
    object_ids.reserve(num_objects_used);
    
    for (int i=0; i<weight_value_vect_size; ++i) {
        if (object_used[i]) object_ids.push_back(i);
    }
    
    return object_ids;
}


int64_t **value_array;

//! Allocate the mem for the dynamic programming solution.
void alloc_dyn_prog_mem(const int weight_value_vect_size, const int W_)
{
    value_array = new int64_t *[weight_value_vect_size+1];
    
    for (int i=0; i <= weight_value_vect_size; ++i) {
        value_array[i] = new int64_t[W_ + 1];
    }
    std::cout << "alloc_dyn_prog_mem: " << ((weight_value_vect_size+1) * (W_+1) * sizeof(int64_t)) / (1024.0 * 1024.0) << "MB allocated.\n";
}

//! free the mem used for the dynamic programming solution.
void free_dyn_prog_mem(const int weight_value_vect_size)
{
    for (int i=0; i <= weight_value_vect_size; ++i) {
        delete [] value_array[i];
    }
    
    delete [] value_array;
}

//! Solve the knapsack problem using dynamic programming.
std::vector<int> knapsack_dyn_prog(const std::vector<std::pair<int, int>> &weight_value_vect_,
                                   const int W_)
{
    const int weight_value_vect_size = weight_value_vect_.size();
    std::vector<bool> object_used(weight_value_vect_size, false);
    int num_objects_used = 0;

    for (int w=0; w <= W_; ++w) {
        value_array[0][w] = 0;
    }
    
    for (int i=1; i <= weight_value_vect_size; ++i) {
        for (int w=0; w <= W_; ++w) {
            const auto &object = weight_value_vect_[i-1]; //Note the -1 here. The objects are 1 indexed.
            const int object_weight = object.first;
            
            if (object_weight > w) {
                value_array[i][w] = value_array[i-1][w];
            }
            else {
                const int object_value = object.second;
                value_array[i][w] = std::max(value_array[i-1][w], value_array[i-1][w-object_weight] + object_value);
            }
        }
    }
    
    //for (int i=0; i<=weight_value_vect_size; ++i)
    //{
    //    for (int w=0; w<=W_; ++w)
    //    {
    //        std::cout << value_array[i][w] << "\t";
    //    }
    //
    //    std::cout << "\n";
    //}
    
    
    {// Trace solution from value_array.
        int i = weight_value_vect_size;
        int w = W_;
        
        do {
            if (value_array[i][w] != value_array[i-1][w])
            {// Object i (1 indexed) contributed to the weight and was therefore used in solution.
                object_used[i-1] = true;
                num_objects_used += 1;
                
                w -= weight_value_vect_[i-1].first;
            }
            
            i -= 1;
        } while (i>0);
    }
    
    // Output
    std::vector<int> object_ids;
    object_ids.reserve(num_objects_used);
    
    for (int i=0; i<weight_value_vect_size; ++i) {
        if (object_used[i]) object_ids.push_back(i);
    }
    
    return object_ids;
}

int64_t calc_knapsack_value(const std::vector<int> &object_ids,
                        const std::vector<std::pair<int, int>> &weight_value_vect_)
{
    int64_t knapsack_value = 0;
    
    for (int object_id : object_ids)
    {
        knapsack_value += weight_value_vect_[object_id].second;
        //std::cout << object_id << " ";
    }
    //std::cout << "\n";
    
    return knapsack_value;
}

const double EXP_TSC_FREQ = 2.89992e+09;
TCRandom<TC_MCG_Lehmer_RandFunc32> rng(987654321);

int main(void)
{
    // === Setup the problem ===//
    //std::cout << "\nProblem 1: \n";
    //const int W = 5;
    //std::vector<std::pair<int, int>> weight_value_vect = {{3, 8}, {4, 12}, {2, 5}};
    
    //std::cout << "\nProblem 2: \n";
    //const int W = 10;
    //const int n = 10;
    //std::vector<std::pair<int, int>> weight_value_vect;
    //for (int i=0; i<n; ++i)
    //{
    //    weight_value_vect.push_back(std::make_pair(rng.next(10), i+1));
    //}
    
    std::cout << "\nProblem 3: \n";
    const int W = 10000;
    const int n = 10000;
    std::vector<std::pair<int, int>> weight_value_vect;
    for (int i=0; i<n; ++i)
    {
        weight_value_vect.push_back(std::make_pair(rng.next(100), i+1));
    }
    
    
    // === Solve the problem ===//
    TCTimer::init_timer(EXP_TSC_FREQ);
    
    
    {// == Greedy
        const double start_time_greedy = TCTimer::get_time();

        const std::vector<int> object_ids_greedy = knapsack_greedy(weight_value_vect, W);
        
        const int64_t value_greedy = calc_knapsack_value(object_ids_greedy, weight_value_vect);
        const double end_time_greedy = TCTimer::get_time();
        const double time_greedy = end_time_greedy - start_time_greedy;
        DBS(value_greedy)
        DBN(time_greedy)
    }
    
    
    alloc_dyn_prog_mem(weight_value_vect.size(), W);
    {// == DP 1
        const double start_time_dp = TCTimer::get_time();
        
        const std::vector<int> object_ids_dyn_prog_simple = knapsack_dyn_prog(weight_value_vect, W);
        
        const int64_t value_dp = calc_knapsack_value(object_ids_dyn_prog_simple, weight_value_vect);
        const double end_time_dp = TCTimer::get_time();
        const double time_dp = end_time_dp - start_time_dp;
        DBS(value_dp)
        DBN(time_dp)
    }
    {// == DP 2 - Do again to check performance after mem available to process.
        const double start_time_dp = TCTimer::get_time();
        
        const std::vector<int> object_ids_dyn_prog_simple = knapsack_dyn_prog(weight_value_vect, W);
        
        const int64_t value_dp = calc_knapsack_value(object_ids_dyn_prog_simple, weight_value_vect);
        const double end_time_dp = TCTimer::get_time();
        const double time_dp = end_time_dp - start_time_dp;
        DBS(value_dp)
        DBN(time_dp)
    }
    free_dyn_prog_mem(weight_value_vect.size());

    return 0;
}
