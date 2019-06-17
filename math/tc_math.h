#ifndef TC_MATH_H
#define TC_MATH_H 1

#include "../defines/tc_defines.h"

#include <cmath>
#include <stdint.h>

namespace tc_math {
    //! Fast integer log2.
    inline int fast_int_log2(const int x) {
        int y;
        __asm__ volatile ("bsrl %1, %0"
                          : "=r" (y) //out
                          : "r" (x) //in
                          );
        return y;
    }

    //! Find the greatest common divisor. Note: gcd(a, 0) is a; gcd(0, b) is b;
    int gcd(int a, int b) {
        return b == 0 ? a : gcd(b, a % b);
    }

    //! Reverse the order of the bytes.
    ALWAYS_INLINE uint32_t byte_swap_32(const uint32_t value) {return __builtin_bswap32(value);}
    
    //! Reverse the order of the bytes.
    ALWAYS_INLINE uint64_t byte_swap_64(const uint64_t value) {return __builtin_bswap64(value);}

    //! Approximate exp by Schraudolph, 1999 - double precision floating point version.
    ALWAYS_INLINE double fast_exp(const double x) noexcept {
        // Based on Schraudolph 1999, A Fast, Compact Approximation of the Exponential Function.
        // - See the improved fast_exp_64 implementation below!
        // - Valid for x in approx range (-700, 700).
        union{double d_; int32_t i_[2];} uid; //This could be moved to the thread scope.
        //BBBD(sizeof(uid)!=8)
        uid.i_[0] = 0;
        uid.i_[1] = int32_t(double((1<<20) / log(2.0)) * x + double((1<<20) * 1023 - 0)); //c=0 for 1.0 at zero.
        return uid.d_;
    }
    
    //! Approximate exp adapted from Schraudolph, 1999 - double precision floating point version.
    ALWAYS_INLINE double fast_exp_64(const double x) noexcept {
        // Based on Schraudolph 1999, A Fast, Compact Approximation of the Exponential Function.
        // - Adapted to use 64-bit integer; reduces staircase effect.
        // - Valid for x in approx range (-700, 700).
        union{double d_; int64_t i_;} uid; //This could be moved to the thread scope.
        //BBBD(sizeof(uid)!=8)
        uid.i_ = int64_t(double((int64_t(1) << 52) / log(2.0)) * x + double((int64_t(1) << 52) * 1023 - 0)); //c=0 for 1.0 at zero.
        return uid.d_;
    }
}

#endif //TC_MATH_H
