#ifndef TC_RANDOM_FUNCS_H
#define TC_RANDOM_FUNCS_H 1

//====================//
//=== TC RNGs ========//
//====================//
//! Stateless [0,2^64) splitmix64 by Daniel Lemire https://github.com/lemire/testingRNG . Useful for seeding RNGs.
ALWAYS_INLINE uint64_t splitmix64_stateless(const uint64_t index) noexcept { //??ns on TC's EC2! 1.3 ns on local.
    uint64_t z = index + UINT64_C(0x9E3779B97F4A7C15);
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}

//! 64-bit Intel RDSEED. Useful for seeding RNGs.
ALWAYS_INLINE uint64_t rdseed64() noexcept { //??ns on TC's EC2! 450 ns on local.
    uint64_t rand;
    unsigned char ok;
    
    asm volatile ("rdseed %0; setc %1"
                  : "=r" (rand), "=qm" (ok));
    
    while (!ok) {
        asm volatile ("pause; rdseed %0; setc %1"
                      : "=r" (rand), "=qm" (ok));
    }
    return rand;
}
//rdseed64():
//      jmp .L6
//  .L3:
//      pause;
//  .L6:
//      rdseed rax; setc dl
//      test dl, dl
//      je .L3
//  ret

//======
//! Lehmer RNG with 64bit multiplier, derived from https://github.com/lemire/testingRNG.
class TC_MCG_Lehmer_RandFunc32 {
public:
    TC_MCG_Lehmer_RandFunc32(const uint32_t seed = 0) {init(seed);}
    
    //!Calc LCG random number in [0,2^32)
    ALWAYS_INLINE uint32_t operator()() noexcept {//??ns on TC's EC2! 1.0 ns on local.
        state_.s128_ *= UINT64_C(0xda942042e4dd58b5);
        return uint32_t(state_.s64_[1]);
    }
    
    void init(const uint32_t seed) {state_.s128_ = (__uint128_t(splitmix64_stateless(seed)) << 64) + splitmix64_stateless(seed + 1);}
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
    
private:
    union{__uint128_t s128_; uint64_t s64_[2];} state_; //Assumes little endian so that s64[0] is the low 64 bits of s128_.
};

//======
//! splitmix 64. High 32 bits of 64 bit random number is used.
class TC_SplitMix_64_RandFunc32 {
public:
    TC_SplitMix_64_RandFunc32(const uint32_t seed = 0) {init(seed);}
    
    //!Calc splitmix random number in [0,2^32)
    ALWAYS_INLINE uint32_t operator()() noexcept {//??ns on TC's EC2! 1.3 ns on local.
        uint64_t z = (state_ += UINT64_C(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return static_cast<uint32_t>((z ^ (z >> 31)) >> 31); //ToDo: Test if it will be faster if last shift is 32 instead of 31.
    }
    
    void init(const uint32_t seed) {state_ = splitmix64_stateless(seed);}
    
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
    
private:
    uint64_t state_;
};

//======
#define PCG32_DEFAULT_STATE  0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT           0x5851f42d4c957f2dULL

//! Derived from pbrt-v3. PCG32 originally from http://www.pcg-random.org under Apache License 2.0. (c) 2014 M.E. O'Neill / pcg-random.
class TC_PCG32_RandFunc32 {
public:
    TC_PCG32_RandFunc32(const uint64_t seed = 0) {init(seed);}
    
    //!Calc PCG32 random number in [0,2^32)
    ALWAYS_INLINE uint32_t operator()() noexcept { //??ns on TC's EC2! 1.5 ns on local.
        const uint64_t oldstate = state_;
        state_ = oldstate * PCG32_MULT + inc_;
        const uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18) ^ oldstate) >> 27);
        const uint32_t rot = oldstate >> 59;
        return (xorshifted >> rot) | (xorshifted << (32 - rot));
    }
    
    void init(const uint64_t seed) {
        state_ = splitmix64_stateless(seed); //PCG32_DEFAULT_STATE;
        inc_ = splitmix64_stateless(seed + 1) | 1; //PCG32_DEFAULT_STREAM;
    }
    
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
    
private:
    uint64_t state_, inc_;
};

//======
// 32-bit RNG using Intel's DRNG CPU instructions. Warning: It is slow! 100x slower than PCG!
class TC_IntelDRNG_RandFunc32 {
public:
    TC_IntelDRNG_RandFunc32(const uint32_t seed = 0) {init(seed);}
    
    //!Intel DRNG random number in [0,2^32)
    ALWAYS_INLINE uint32_t operator()() noexcept {//??ns on TC's EC2! 120ns on local!!!
        uint32_t rand;
        unsigned char ok;
        do {
            asm volatile ("rdrand %0; setc %1"
                          : "=r" (rand), "=qm" (ok));
        } while (!ok);
        return rand;
    }
    
    void init(const uint32_t seed) {} //No seeding required.
    
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
};

//====================//
//====================//
//====================//


//====================//
//=== TCRandom =======//
//====================//
//#define TC_RAND_REJECT_BIAS 1

/*! Random number generator template class.
 * Usage:
 *   TCRandom<TC_PCG32_RandFunc> rng_;
 *   r = rngRef_.next_double() */
template<typename RandFunc>
class TCRandom {
public:
    TCRandom(const uint32_t seed = 0) : rf_(seed), rnd_bits_(0), rnd_bit_count_(0) {}
    void seed(const uint32_t seed) {rf_.init(seed);}
    void discard(const uint32_t count) noexcept {for (uint32_t i=0; i<count; ++i) rf_();}
    
    //! Shuffle the provided sequence of numbers. Works for all value types.
    template<class T>
    ALWAYS_INLINE void shuffle(T * const sequence, const uint32_t n) noexcept {
        for (uint32_t i = (n-1); i > 0; --i) {
            const uint32_t r = next(i+1);
            const T tmp = sequence[r]; sequence[r] = sequence[i]; sequence[i] = tmp;
        }
    }
    
    //! Generate a random sequence (a shuffle) from [0,s). Works for all number value types.
    template<class T>
    ALWAYS_INLINE void next_sequence(T * const sequence, const uint32_t s) noexcept {
        for (uint32_t i=0; i<s; ++i) sequence[i]=i;
        shuffle(sequence, s);
    }
    
    //!Get uniform uint32_t
    ALWAYS_INLINE uint32_t next() noexcept {return rf_();}
    
    //!Get uniform uint32_t in [0,x) :
    ALWAYS_INLINE uint32_t next(const uint32_t s) noexcept {
        //Daniel Lemire https://arxiv.org/abs/1805.10941
        uint64_t m = uint64_t(rf_()) * s;
#ifdef TC_RAND_REJECT_BIAS
        uint32_t leftover = m & uint32_t((uint64_t(1) << rf_.num_bits()) - 1);
        if (leftover < s) {
            const uint32_t threshold = /*-s % s;*/ uint32_t((uint64_t(1) << rf_.num_bits()) - s) % s;
            while (leftover < threshold) {
                m = uint64_t(rf_()) * s;
                leftover = m & uint32_t((uint64_t(1) << rf_.num_bits()) - 1);
            }
        }
#endif
        const uint32_t r = static_cast<uint32_t>(m >> rf_.num_bits());
        BBBD(r>=s)//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform uint32_t in [a, b) :
    ALWAYS_INLINE uint32_t next(const uint32_t a, const uint32_t b) noexcept {
        const uint32_t r = a + next(b-a);
        BBBD((r<a)||(r>=b))//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform float in [0.0f..1.0f) - Note: Due to limited precision a 1.0f is sometimes generated.
    ALWAYS_INLINE float next_float() noexcept {
        const float r=(float(rf_())+0.5f) * float(RandFunc::recip_max_plus_one());
        BBBD(r>=1.0f)//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform double in [0..1.0) -
    ALWAYS_INLINE double next_double() noexcept {
        const double r=(double(rf_())+0.5) * RandFunc::recip_max_plus_one();
        BBBD(r>=1.0)//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform double in [0, s) -
    ALWAYS_INLINE double next_double(const double s) noexcept {
        const double r=next_double() * s;
        BBBD(r>=s)//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform double in [a, b) -
    ALWAYS_INLINE double next_double(const double a, const double b) noexcept {
        const double r=a + next_double() * (b-a);
        BBBD((r<a)||(r>=b))//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get random boolean - 1.05 ns when reusing rng_ bits!
    ALWAYS_INLINE bool next_boolean() noexcept {
        if (rnd_bit_count_ == 0) {
            rnd_bits_ = next();
            rnd_bit_count_ = 8;//rf_.num_bits() - 1; //bits in cache post return! 8 seems to be a performance sweet spot.
        } else {
            rnd_bits_ >>= 1;
            rnd_bit_count_ -= 1;
        }
        return rnd_bits_ & 1;
    }
    
    //!Get a random sample from the triangle distribution with mean at 0.5.
    ALWAYS_INLINE double next_triangular() noexcept {
        return (next_double()+next_double()) * 0.5;
    }
    
    //!Get a random sample from an approx Gaussian distribution with mean at 0.5.
    ALWAYS_INLINE double next_gaussian() noexcept {
        constexpr double one_third = 1.0/3.0;
        return (next_double()+next_double()+next_double()) * one_third;
    }
    
    constexpr int num_bits() const noexcept {return rf_.num_bits();}
    
private:
    RandFunc rf_;
    
    uint32_t rnd_bits_; //!< The random bits cached for next_boolean()
    uint32_t rnd_bit_count_;//!< The remaining cached random bits.
};


//std::random_device cpp_rand_device;

//const uint32_t fast_rng_seed_ = cpp_rand_device();
const uint32_t fast_rng_seed_ = 0;
TCRandom<TC_MCG_Lehmer_RandFunc32> fast_rng_(fast_rng_seed_);
//TCRandom<TC_SplitMix_64_RandFunc32> fast_rng_(fast_rng_seed_);

//const uint32_t rng_seed_ = cpp_rand_device();
const uint32_t rng_seed_ = 0;
TCRandom<TC_PCG32_RandFunc32> good_rng_(rng_seed_);

//const uint32_t drng_seed_ = cpp_rand_device();
const uint32_t drng_seed_ = 0;
TCRandom<TC_IntelDRNG_RandFunc32> drng_rng(drng_seed_);

//====================//
//====================//
//====================//

#endif
