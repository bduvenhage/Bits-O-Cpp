//====================//
//=== TC Timer =======//
//====================//
/*!Singleton/Static Timer that uses 'timeofday' and the TSC timer. A modern constant_tsc and nonstop_tsc CPU is assumed.
 * Usage:
 *   TCTimer::init_timer(2500000000); //Initial guess of performance timer freq.
 *   time = TCTimer::get_time(); //Return the number of seconds since init_timer(). Based on gettimeofday!
 *   ... After some time has passed.
 *   TCTimer::sync_tsc_time(); //Update the initial guess of performance timer freq.
 *   time = TCTimer::get_tsc_time(); //Return the number of seconds since init_timer. Based on TSC! */
class TCTimer
{
public:
    //!Re-init the timers to the current time.
    static void init_timer(double est_clock_freq) noexcept
    {
        seconds_per_tick_ = 1.0 / est_clock_freq;
        init_time_ = get_tod_seconds();
        init_tick_ = get_tsc_tick_p(chip_, core_);
    }
    
    //!Update the TSC' secondsPerTick_ based on actual seconds and ticks passed since init. ALSO returns the number of seconds since initTimer() based on gettimeofday!
    static double sync_tsc_time() noexcept// - 130000ns on TC's EC2! Local performance = 50ns
    {
        const double dTime = get_tod_seconds() - init_time_;
        const uint64_t dTicks = get_tsc_tick_p(chip_, core_) - init_tick_;
        seconds_per_tick_ = dTime / dTicks;
        return dTime;
    }
    
    //!Return the number of seconds since initTimer(). Based on gettimeofday! - 130000ns on TC's EC2! Local performance = 30ns
    static ALWAYS_INLINE double get_time() noexcept//a.k.a. getTODTime()
    {
        return (get_tod_seconds()-init_time_);
    }
    
    /*!Return the number of seconds since initTimer. Based on TSC!
     @remark The TSC is incremented by the base multiplier once every reference clock. In other words, the same value could be returned multiple times!
     */
    static ALWAYS_INLINE double get_tsc_time() noexcept//Takes 9.4ns on TC's EC2! Local performance = 7.4ns.
    {
        return int64_t(get_tsc_tick()-init_tick_) * seconds_per_tick_;
    }
    
    /*!Return the number of seconds since initTimer. Based on TSC !
     @remark The TSC is incremented by the base multiplier once every reference clock. In other words, the same value could be returned multiple times!
     */
    static ALWAYS_INLINE double get_tsc_time_p(int &chip, int &core) noexcept//Takes ?.?ms on TC's EC2! Local performance = 16ns.
    {
        return int64_t(get_tsc_tick_p(chip, core)-init_tick_) * seconds_per_tick_;
    }
    
    //!Return the estimated number of seconds per clock tick.
    static ALWAYS_INLINE double get_seconds_per_tick() noexcept
    {
        return seconds_per_tick_;
    }
    
    //!Return the estimated CPU clock frequency.
    static ALWAYS_INLINE double get_clock_freq() noexcept
    {
        return 1.0 / seconds_per_tick_;
    }
    
    //!Get the chip number on which syncTSCTime() last executed.
    static ALWAYS_INLINE int get_chip() noexcept
    {
        return chip_;
    }
    
    //!Get the core number on which syncTSCTime() last executed.
    static ALWAYS_INLINE int get_core() noexcept
    {
        return core_;
    }
    
    /*!Return the number of clock ticks since some past event - a modern constant_tsc and nonstop_tsc CPU is assumed.
     @remark The TSC is incremented by the base multiplier once every reference clock. In other words, the same value could be returned multiple times!
     */
    static ALWAYS_INLINE uint64_t get_tsc_tick() noexcept
    {
        //#ifdef __x86_64
        uint64_t timelo, timehi;
        __asm__ volatile ("rdtsc" : "=a" (timelo), "=d" (timehi));
        return (timehi << 32) | timelo;
        //OR
        //return __rdtsc();//slightly slower than asm rdtsc?
    }
    
    //!Return the number of TSC ticks since some past event. Under Linux the additional values returned by rdtscp contains the chip and core that the instruction executed on.
    static ALWAYS_INLINE uint64_t get_tsc_tick_p(int &chip, int &core) noexcept
    {
        uint64_t timelo, timehi;
        uint32_t chx;
        __asm__ volatile("rdtscp" : "=a" (timelo), "=d" (timehi), "=c" (chx));
        chip = (chx & 0xFFF000)>>12;
        core = (chx & 0xFFF);
        return (timehi << 32) | timelo;
    }
    
private:
    //!Return the number of seconds since some past event. Based on gettimeofday() - 130000ns on TC's EC2! Local performance = 30ns.
    static ALWAYS_INLINE double get_tod_seconds() noexcept
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec + tv.tv_usec*0.000001;
    }
    
    //!Number of seconds per TSC tick. Updated in sync_tsc_time().
    static double seconds_per_tick_;
    //!Chip number on which sync_tsc_time() last executed.
    static int chip_;
    //!Core number on which sync_tsc_time() last executed.
    static int core_;
    
    //!Reference time in seconds from gettimeofday().
    static double init_time_;
    //!Reference TSC tick.
    static uint64_t init_tick_;
};

double TCTimer::seconds_per_tick_=0.0;
double TCTimer::init_time_=0.0;
uint64_t TCTimer::init_tick_=0;
int TCTimer::chip_=0;
int TCTimer::core_=0;
