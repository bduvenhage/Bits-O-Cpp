#ifndef TC_SIMULATED_ANNEALING
#define TC_SIMULATED_ANNEALING 1

const double T = 50.0 * (1.000001 - pow(time / max_time_, 1.0));
const double p = exp((solution_vec[si].length_ - new_solution.length_)/T);

if (rng_.next_double() < p)
//if (new_solution.length_ < solution_vec[si].length_)
{

}

#endif //TC_SIMULATED_ANNEALING
