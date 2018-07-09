
//======
//! The typical LCG implemented by C++ compilers. High 16 bits of the 32 bit LCG is used here.
class TC_LCG_STD_RandFunc16 {
public:
    TC_LCG_STD_RandFunc16(const uint32_t seed = 0) {init(seed);}
    
    //!Calc LCG random number in [0,2^16)
    ALWAYS_INLINE uint32_t operator()() noexcept { //??ns on TC's EC2! 1.2 ns on local.
        //seed_ = 1664525 * seed_ + 1013904223; //numerical recipes
        state_ = 1103515245 * state_ + 12345; //glibc
        //seed_ = 214013 * seed + 2531011; //msvs
        return state_ >> 16;
    }
    
    void init(const uint32_t seed) {state_=splitmix64_stateless(seed);}
    static constexpr double max_plus_one() noexcept {return 65536.0;} //0x1p16
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 65536.0);} //1.0/0x1p16
    static constexpr int num_bits() noexcept {return 16;}
    
private:
    uint32_t state_;
};

//======
//! XOR Shift 128. High 16 bits of 32 bit random number is used.
class TC_XOR_SHIFT_128_RandFunc16 {
public:
    TC_XOR_SHIFT_128_RandFunc16(const uint32_t seed = 0) {init(seed);}
    
    //!Calc XOR shift random number in [0,2^16)
    ALWAYS_INLINE uint32_t operator()() noexcept {//??ns on TC's EC2! 1.2 ns on local.
        const uint32_t t = x_^(x_<<11);
        x_=y_; y_=z_; z_=w_;
        w_ = (w_ ^ (w_ >> 19)) ^ (t ^ (t >> 8));
        return w_ >> 16;
    }
    
    void init(const uint32_t seed) {x_ = splitmix64_stateless(seed); y_ = splitmix64_stateless(seed+1); z_ = splitmix64_stateless(seed+2); w_ = splitmix64_stateless(seed+3);}
    static constexpr double max_plus_one() noexcept {return 65536.0;} //0x1p16
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 65536.0);} //1.0/0x1p16
    static constexpr int num_bits() noexcept {return 16;}
    
private:
    uint32_t x_, y_, z_, w_;
};

//======
//! XOR Shift 128+. High 32 bits of 64 bit random number is used.
class TC_XOR_SHIFT_128_Plus_RandFunc32 {
public:
    TC_XOR_SHIFT_128_Plus_RandFunc32(const uint32_t seed = 0) {init(seed);}
    
    //!Calc XOR shift random number in [0,2^32)
    ALWAYS_INLINE uint32_t operator()() noexcept { //??ns on TC's EC2! 1.35 ns on local.
        uint64_t s1 = k1_;
        const uint64_t s0 = k2_;
        k1_ = s0;
        s1 ^= s1 << 23; // a
        k2_ = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
        return (k2_ + s0) >> 32;
    }
    
    void init(const uint32_t seed) {k1_ = splitmix64_stateless(seed); k2_ = splitmix64_stateless(seed + 1);}
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
    
private:
    uint64_t k1_, k2_;
};

//======
//! XOR Shift 64. High 32 bits of 64 bit random number is used.
class TC_XOR_SHIFT_64_RandFunc32 {
public:
    TC_XOR_SHIFT_64_RandFunc32(const uint32_t seed = 0) {init(seed);}
    
    //!Calc XOR shift random number in [0,2^32)
    ALWAYS_INLINE uint32_t operator()() noexcept {//??ns on TC's EC2! 2.00 ns on local.
        const uint64_t result = state_ * 0xd989bcacc137dcd5ull;
        state_ ^= state_ >> 11;
        state_ ^= state_ << 31;
        state_ ^= state_ >> 18;
        return result >> 32ull;
    }
    
    void init(const uint32_t seed) {state_ = splitmix64_stateless(seed);}
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
    
private:
    uint64_t state_;
};

//======
class TC_MT32_RandFunc32 {
public:
    TC_MT32_RandFunc32(const uint32_t seed = 0) {init(seed);}
    
    ALWAYS_INLINE uint32_t operator()() noexcept { //??ns on TC's EC2! 3.2 ns on local.
        if (index_ == 0) generate();
            uint32_t y = MT_[index_];
        y ^= y >> 11; y ^= (y << 7)  & 2636928640UL; y ^= (y << 15) & 4022730752UL; y ^= y >> 18;
        index_ = (index_ == 623) ? 0 : (index_ + 1);
        return y;
    }
    
    void init(const uint32_t seed) {
        MT_[0] = seed;
        for (int i=1; i<624; ++i) {MT_[i] = (1812433253UL * (MT_[i-1] ^ (MT_[i-1] >> 30)) + i);}
        index_ = 0;
    }
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
    
private:
    void generate() noexcept {
        const uint32_t MULT[] = {0, 2567483615UL};
        
        for (int i=0; i<227; ++i) {
            const uint32_t y = (MT_[i] & 0x8000000UL) + (MT_[i+1] & 0x7FFFFFFFUL);
            MT_[i] = MT_[i+397] ^ (y >> 1);
            MT_[i] ^= MULT[y&1];
        }
        for (int i=227; i<623; ++i) {
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
class CPP_MT32_RandFunc32 {
public:
    CPP_MT32_RandFunc32(const uint32_t seed = 0) {init(seed);}
    
    ALWAYS_INLINE uint32_t operator()() noexcept { //??ns on TC's EC2! 5.4 ns on local.
        return rndGen_();
    }
    
    void init(const uint32_t seed) {rndGen_.seed(seed);}
    static constexpr double max_plus_one() noexcept {return 4294967296.0;} //0x1p32
    static constexpr double recip_max_plus_one() noexcept {return (1.0 / 4294967296.0);} //1.0/0x1p32
    static constexpr int num_bits() noexcept {return 32;}
    
private:
    std::mt19937 rndGen_;
};
