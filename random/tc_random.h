//====================//
//=== TC RNGs ========//
//====================//
class TC_LCG_RandFunc
{
public:
    TC_LCG_RandFunc(const uint32_t seed = 0) {init(seed);}
    
    //!Calc LCG random number in [0,2^16)
    ALWAYS_INLINE uint32_t operator()()  //Takes ??ns on TC's EC2! Local performance = 1.0 ns
    {
        //seed_ = 1664525 * seed_ + 1013904223; //numerical recipes
        seed_ = 1103515245 * seed_ + 12345; //glibc
        //seed_ = 214013 * seed + 2531011; //msvs
        
        return seed_ >> 16;
    }
    
    static constexpr double max_plus_one() {return 65536.0;} //0x1p16
    static constexpr double recip_max_plus_one() {return (1.0 / 65536.0);} //1.0/0x1p16
    static constexpr int num_bits() {return 16;}
    
private:
    void init(const uint32_t seed) { seed_=seed; }
    
    uint32_t seed_;
};

//======
class TC_XOR_SHIFT_96_RandFunc
{
public:
    TC_XOR_SHIFT_96_RandFunc(const uint32_t seed = 0) {init(seed);}
    
    //!Calc XOR shift random number in [0,2^16)
    ALWAYS_INLINE uint32_t operator()()  //Takes ??ns on TC's EC2! Local performance = 1.2 ns
    {
        x_ ^= x_ << 16;
        x_ ^= x_ >> 5;
        x_ ^= x_ << 1;
        
        unsigned int t = x_;
        x_ = y_;
        y_ = z_;
        z_ = t ^ x_ ^ y_;
        
        return z_ >> 16;
    }
    
    static constexpr double max_plus_one() {return 65536.0;} //0x1p16
    static constexpr double recip_max_plus_one() {return (1.0 / 65536.0);} //1.0/0x1p16
    static constexpr int num_bits() {return 16;}
    
private:
    void init(uint32_t seed)
    {
        //x_ = 123456789; y_ = 362436069; z_ = 521288629;
        //x_ = seed; y_ = 362436069; z_ = 521288629;
        //x_ = 31103110, y_ = 123456789, z_ = 521288629;
        //x_ = 314159261; y_ = 358979323; z_ = 846264338;
        x_ = seed = 1812433253u * (seed ^ (seed >> 30));
        y_ = seed = 1812433253u * (seed ^ (seed >> 30)) + 1;
        z_ = seed = 1812433253u * (seed ^ (seed >> 30)) + 2;
    }
    //inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 10) mix(); }
    //inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    
    uint32_t x_, y_, z_;
};

//======
class TC_XOR_SHIFT_128_RandFunc
{
public:
    TC_XOR_SHIFT_128_RandFunc(const uint32_t seed = 0) {init(seed);}
    
    //!Calc XOR shift random number in [0,2^16)
    ALWAYS_INLINE uint32_t operator()()  //Takes ??ns on TC's EC2! Local performance = 1.1 ns
    {
        const uint32_t t = x_^(x_<<11);
        x_=y_; y_=z_; z_=w_;
        w_ = (w_ ^ (w_ >> 19)) ^ (t ^ (t >> 8));
        
        return w_ >> 16;
    }
    
    static constexpr double max_plus_one() {return 65536.0;} //0x1p16
    static constexpr double recip_max_plus_one() {return (1.0 / 65536.0);} //1.0/0x1p16
    static constexpr int num_bits() {return 16;}
    
private:
    void init(uint32_t seed)
    {
        //x_ = 123456789; y_ = 362436069; z_ = 521288629; w_ = 88675123;
        //x_ = seed; y_ = 362436069; z_ = 521288629; w_ = 786232308;
        //x_ = 31103110, y_ = 123456789, z_ = 521288629, w_ = 88675123;
        //x_ = 314159261; y_ = 358979323; z_ = 846264338; w_ = 327950288;
        x_ = seed = 1812433253u * (seed ^ (seed >> 30));
        y_ = seed = 1812433253u * (seed ^ (seed >> 30)) + 1;
        z_ = seed = 1812433253u * (seed ^ (seed >> 30)) + 2;
        w_ = seed = 1812433253u * (seed ^ (seed >> 30)) + 3;
    }
    //inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 10) mix(); }
    //inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    
    
    uint32_t x_, y_, z_, w_;
};

//======
//Derived from pbrt-v3. PCG32 originally from http://www.pcg-random.org under Apache License 2.0. (c) 2014 M.E. O'Neill / pcg-random.
#define PCG32_DEFAULT_STATE  0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT           0x5851f42d4c957f2dULL

class TC_PCG32_RandFunc
{
public:
    TC_PCG32_RandFunc(const uint64_t seed = 0) {init(seed);}
    
    //!Calc PCG32 random number in [0,2^32)
    ALWAYS_INLINE uint32_t operator()()  //Takes ??ns on TC's EC2! Local performance = 1.7 ns
    {
        const uint64_t oldstate = state_;
        state_ = oldstate * PCG32_MULT + inc_;
        const uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
        const uint32_t rot = (uint32_t)(oldstate >> 59u);
        return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
    }
    
    static constexpr double max_plus_one() {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() {return 32;}
    
private:
    void init(const uint64_t seed)
    {
        state_ = PCG32_DEFAULT_STATE;
        inc_ = PCG32_DEFAULT_STREAM;
        
        if (seed!=0)
        {
            state_ = 0u; inc_ = (seed << 1u) | 1u; operator()();
            state_ += PCG32_DEFAULT_STATE; operator()();
            //OR
            //const uint32_t jump=seed & 0xFFF; //Sequence jump of 0 - 4095 numbers; Up to ??ms!
            //for (uint32_t i=0; i<jump; ++i) {operator()();}
        }
    }
    
    uint64_t state_, inc_;
};

//======
class TC_MT32_RandFunc
{
public:
    TC_MT32_RandFunc(const uint32_t seed = 0) {init(seed);}
    
    ALWAYS_INLINE uint32_t operator()()  //Takes ??ns on TC's EC2! Local performance = 3.2 ns.
    {
        if (index_ == 0) generate();
        
        uint32_t y = MT_[index_];
        
        y ^= y >> 11;
        y ^= y << 7  & 2636928640UL;
        y ^= y << 15 & 4022730752UL;
        y ^= y >> 18;
        
        index_ = (index_ == 623) ? 0 : (index_ + 1);
        
        return y;
    }
    
    static constexpr double max_plus_one() {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() {return 32;}
    
private:
    void init(const uint32_t seed)
    {
        MT_[0] = seed;
        for (int i=1; i<624; ++i) {MT_[i] = (1812433253UL * (MT_[i-1] ^ (MT_[i-1] >> 30)) + i);}
        index_ = 0;
    }
    
    void generate()
    {
        const uint32_t MULT[] = {0, 2567483615UL};
        
        for (int i=0; i<227; ++i)
        {
            const uint32_t y = (MT_[i] & 0x8000000UL) + (MT_[i+1] & 0x7FFFFFFFUL);
            MT_[i] = MT_[i+397] ^ (y >> 1);
            MT_[i] ^= MULT[y&1];
        }
        
        for (int i=227; i<623; ++i)
        {
            const uint32_t y = (MT_[i] & 0x8000000UL) + (MT_[i+1] & 0x7FFFFFFFUL);
            MT_[i] = MT_[i-227] ^ (y >> 1);
            MT_[i] ^= MULT[y&1];
        }
        
        const uint32_t y = (MT_[623] & 0x8000000UL) + (MT_[0] & 0x7FFFFFFFUL);
        MT_[623] = MT_[623-227] ^ (y >> 1);
        MT_[623] ^= MULT[y&1];
    }
    
    uint32_t MT_[624];
    int index_;
};

//======
class CPP_MT32_RandFunc
{
public:
    CPP_MT32_RandFunc(const uint32_t seed = 0) {init(seed);}
    
    ALWAYS_INLINE uint32_t operator()()  //Takes ??ns on TC's EC2! Local performance = 5.2 ns.
    {
        //return uniformIntDist_(rndGen_);
        return rndGen_();
    }
    
    static constexpr double max_plus_one() {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() {return 32;}
    
private:
    void init(const uint32_t seed) {rndGen_.seed(seed);}
    
    std::mt19937 rndGen_;
    //std::uniform_int_distribution<uint32_t> uniformIntDist_;
};
//====================//
//====================//
//====================//


//====================//
//=== TCRandom =======//
//====================//
/*!Random number generator template class.
 * Usage:
 *   TCRandom<TC_PCG32_RandFunc> rng_;
 *   r = rngRef_.next_double() */

//#define TC_RAND_USE_SCALED_DOUBLE 1
//#define TC_RAND_REJECT_BIAS 1

template<typename RandFunc>
class TCRandom
{
public:
    TCRandom(const uint32_t seed = 0) :
    rf_(seed), rnd_bits_(0), rnd_bit_count_(0)
    {}
    
    //! Dump some random numbers.
    void skip(const uint32_t count)
    {
        for (uint32_t i=0; i<count; ++i)
        {
            rf_();
        }
    }
    
    //! Shuffle the provided sequence of numbers. Works for all value types.
    template<class T>
    ALWAYS_INLINE void shuffle(T * const sequence, const uint32_t n)
    {
        for (uint32_t i = (n-1); i > 0; --i)
        {
            const uint32_t r = next(i+1);
            
            const T tmp = sequence[r];
            sequence[r] = sequence[i];
            sequence[i] = tmp;
        }
    }
    
    //! Generate a random sequence (a shuffle) from [0,x). Works for all number value types.
    template<class T>
    ALWAYS_INLINE void next_sequence(T * const sequence, const uint32_t x)
    {
        for (uint32_t i=0; i<x; ++i) sequence[i]=i;
        shuffle(sequence, x);
    }
    
    //!Get uniform uint32_t
    ALWAYS_INLINE uint32_t next()
    {
        return rf_();
    }
    
    //!Get uniform uint32_t in [0,x) :
    ALWAYS_INLINE uint32_t next(const uint32_t s)
    {
#ifdef TC_RAND_USE_SCALED_DOUBLE
        const uint32_t r = next_double() * s;
#else
        //Standard slow modulus approach.
        //const uint32_t r = rf_() % s;
        
        //Daniel Lemire https://arxiv.org/abs/1805.10941
        uint64_t m = uint64_t(rf_()) * s;
#ifdef TC_RAND_REJECT_BIAS
        uint32_t leftover = m & uint32_t((uint64_t(1) << rf_.num_bits()) - 1);
        if (leftover < s) {
            const uint32_t threshold = uint32_t((uint64_t(1) << rf_.num_bits()) - s) % s;
            while (leftover < threshold) {
                m = uint64_t(rf_()) * s;
                leftover = m & uint32_t((uint64_t(1) << rf_.num_bits()) - 1);
            }
        }
#endif
        const uint32_t r = (m >> rf_.num_bits());
#endif
        BBBD(r>=s)//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform uint32_t in [a, b) :
    ALWAYS_INLINE uint32_t next(const uint32_t a, const uint32_t b)
    {
        const uint32_t r = a + next(b-a);
        BBBD((r<a)||(r>=b))//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform float in [0.0f..1.0f) - Note: Due to limited precision a 1.0f is sometimes generated.
    ALWAYS_INLINE float next_float()
    {
        const float r=(float(rf_())+0.5f) * float(RandFunc::recip_max_plus_one());
        BBBD(r>=1.0f)//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform double in [0..1.0) -
    ALWAYS_INLINE double next_double()
    {
        const double r=(double(rf_())+0.5) * RandFunc::recip_max_plus_one();
        BBBD(r>=1.0)//Check the random limits when TCDEBUG is defined.
        return r;
    }
    
    //!Get uniform double in [0, s) -
    ALWAYS_INLINE double next_double(const double s)
    {
        return next_double() * s;
    }
    
    //!Get uniform double in [a, b) -
    ALWAYS_INLINE double next_double(const double a, const double b)
    {
        return a + next_double() * (b-a);
    }
    
    //!Get random boolean - 1.05 ns when reusing rng_ bits!
    ALWAYS_INLINE bool next_boolean()
    {
        //return (rf_() & (1<<12)) != 0;
        
        if (rnd_bit_count_ == 0)
        {
            rnd_bits_ = next();
            rnd_bit_count_ = 8;//rf_.num_bits() - 1; //bits left over after the below return! 8 seems to work well.
            return rnd_bits_ & 1;
        } else
        {
            rnd_bits_ >>= 1;
            rnd_bit_count_ -= 1;
            return rnd_bits_ & 1;
        }
    }
    
    //!Get a random sample from the triangle distribution with mean at 0.5.
    ALWAYS_INLINE double next_triangular()
    {
        return (next_double()+next_double()) * 0.5;
    }
    
    //!Get a random sample from an approx Gaussian distribution with mean at 0.5.
    ALWAYS_INLINE double next_gaussian()
    {
        constexpr double one_third = 1.0/3.0;
        return (next_double()+next_double()+next_double()) * one_third;
    }
    
    constexpr const int num_bits() const {return rf_.num_bits();}
    
private:
    RandFunc rf_;
    
    uint32_t rnd_bits_;
    int8_t rnd_bit_count_;
};

//std::random_device cpp_rand_device;

//const uint32_t fast_rng_seed_ = cpp_rand_device();
const uint32_t fast_rng_seed_ = 123456789;
TCRandom<TC_LCG_RandFunc> fast_rng_(fast_rng_seed_);
//TCRandom<TC_XOR_SHIFT_128_RandFunc> fast_rng_(fast_rng_seed_);
//TCRandom<TC_XOR_SHIFT_96_RandFunc> fast_rng_(fast_rng_seed_);

//const uint32_t rng_seed_ = cpp_rand_device();
const uint32_t rng_seed_ = 123456789;
TCRandom<TC_PCG32_RandFunc> rng_(rng_seed_);
//TCRandom<TC_MT32_RandFunc> rng_(rng_seed_);//
//TCRandom<CPP_MT32_RandFunc> rng_(rng_seed_);

//====================//
//====================//
//====================//


