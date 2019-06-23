#ifndef TC_TIMER_H
#define TC_TIMER_H 1

#include "../defines/tc_defines.h"
#include <cstdint>
#include <sys/time.h>

//====================//
//=== TC Timer =======//
//====================//
/*!
 * Singleton/Static Timer that uses 'timeofday' and the TSC timer. A modern constant_tsc+nonstop_tsc (invariante_tsc)
 * CPU is assumed.
 * EXAMPLE Usage:
 *   TCTimer::init_timer(2500000000); //Initial guess of performance timer freq.
 *   time = TCTimer::get_time(); //Return the number of seconds since init_timer(). Based on gettimeofday!
 *   ... After some time has passed.
 *   TCTimer::sync_tsc_time(); //Update the initial guess of performance timer freq.
 *   time = TCTimer::get_tsc_time(); //Return the number of seconds since init_timer. Based on TSC!
 *   //TCTimer::get_tsc_time() is much quicker than TCTimer::get_time()!
 */


// Ek is lief vir jou Pappa! [van Elizabeth Duvenhage, 2019-06-23 (Gr.2).]


class TCTimer {
public:
    //!Re-init the timers to the current time.
    static void init_timer(double est_clock_freq) noexcept {
        seconds_per_tick_ = 1.0 / est_clock_freq;
        init_time_ = _get_tod_seconds_since_epoch();
        init_tick_ = _get_tsc_ticks_since_reset_p(chip_, core_);
    }
    
    /*!
     * Update the TSC' secondsPerTick_ based on actual seconds and ticks passed since init.
     * \remark Takes 130000ns on TC's EC2! Local performance = 50ns. ALSO returns the number
     * of seconds since initTimer() based on gettimeofday!
     */
    static double sync_tsc_time() noexcept {
        const double dTime = _get_tod_seconds_since_epoch() - init_time_;
        const uint64_t dTicks = _get_tsc_ticks_since_reset_p(chip_, core_) - init_tick_;
        seconds_per_tick_ = dTime / dTicks;
        return dTime;
    }
    
    /*!
     * Return the number of seconds since initTimer(). Based on gettimeofday!
     * \remark Takes 130000ns on TC's EC2! Local performance = 30ns
     */
    static ALWAYS_INLINE double get_time() noexcept {return (_get_tod_seconds_since_epoch()-init_time_);}
    
    /*!
     * Return the number of seconds since initTimer. Based on TSC!
     * \remark Takes 9.4ns on TC's EC2! Local performance = 7.4ns. The TSC is incremented by
     * the base multiplier once every reference clock. In other words, the same value could
     * be returned multiple times!
     */
    static ALWAYS_INLINE double get_tsc_time() noexcept {
        return (_get_tsc_ticks_since_reset() - init_tick_) * seconds_per_tick_;
    }
    
    //!Return the number of seconds since initTimer. Based on TSC & fenced!
    static ALWAYS_INLINE double get_tsc_time_fenced() noexcept {
        return (_get_tsc_ticks_since_reset_fenced() - init_tick_) * seconds_per_tick_;
    }

    /*!
     * Return the number of ticks since initTimer. Based on TSC!
     * \remark The TSC is incremented by the base multiplier once every reference clock. In other words,
     * the same value could be returned multiple times!
     */
    static ALWAYS_INLINE uint64_t get_tsc_ticks() noexcept {
        return (_get_tsc_ticks_since_reset() - init_tick_);
    }

    //!Return the number of ticks since initTimer. Based on TSC & fenced!
    static ALWAYS_INLINE uint64_t get_tsc_ticks_fenced() noexcept {
        return (_get_tsc_ticks_since_reset_fenced() - init_tick_);
    }

    /*!
     * Return the number of seconds since initTimer. Based on TSC !
     * \remark Takes ?.?ms on TC's EC2! Local performance = 16ns. The TSC is incremented by the base
     * multiplier once every reference clock. In other words, the same value could be returned multiple
     * times!
     */
    static ALWAYS_INLINE double get_tsc_time_p(int &chip, int &core) noexcept {
        return (_get_tsc_ticks_since_reset_p(chip, core) - init_tick_) * seconds_per_tick_;
    }

    //!Return the number of seconds since initTimer. Based on TSC & fenced!
    static ALWAYS_INLINE double get_tsc_time_p_fenced(int &chip, int &core) noexcept {
        return (_get_tsc_ticks_since_reset_p_fenced(chip, core) - init_tick_) * seconds_per_tick_;
    }

    //!Return the estimated number of seconds per clock tick.
    static ALWAYS_INLINE double get_seconds_per_tick() noexcept {return seconds_per_tick_;}
    
    //!Return the estimated CPU clock frequency.
    static ALWAYS_INLINE double get_clock_freq() noexcept {return 1.0 / seconds_per_tick_;}
    
    //!Get the chip number on which syncTSCTime() last executed.
    static ALWAYS_INLINE int get_chip() noexcept {return chip_;}
    
    //!Get the core number on which syncTSCTime() last executed.
    static ALWAYS_INLINE int get_core() noexcept {return core_;}
    
    /*!
     * Return the number of clock ticks since some past event - a modern constant_tsc and nonstop_tsc CPU is assumed.
     * \remark The TSC is incremented by the base multiplier once every reference clock. In other words, the same value
     * could be returned multiple times! RDTSC reads the 64-bit TSC value into EDX:EAX. The high-order 32 bits of
     * each of RAX and RDX are cleared. The RDTSC instruction is not a serializing instruction. It does not necessarily
     * wait until all previous instructions have been executed before reading the counter. Similarly, subsequent
     * instructions may begin execution before the read operation is performed.
     */
    static ALWAYS_INLINE uint64_t _get_tsc_ticks_since_reset() noexcept {
        uint32_t countlo, counthi;
        
        __asm__ volatile ("RDTSC" : "=a" (countlo), "=d" (counthi));
        return (uint64_t(counthi) << 32) | countlo;
    }
    
    static ALWAYS_INLINE uint64_t _get_tsc_ticks_since_reset_fenced() noexcept {
        uint32_t countlo, counthi;
        
        __asm__ volatile ("lfence;RDTSC;lfence" : "=a" (countlo), "=d" (counthi));
        // Notes:
        // If software requires RDTSC to be executed only after all previous instructions have executed and all previous loads are globally visible,1 it can execute LFENCE immediately before RDTSC.
        // If software requires RDTSC to be executed only after all previous instructions have executed and all previous loads and stores are globally visible, it can execute the sequence MFENCE;LFENCE immediately before RDTSC.
        // If software requires RDTSC to be executed prior to execution of any subsequent instruction (including any memory accesses), it can execute the sequence LFENCE immediately after RDTSC.
        
        return (uint64_t(counthi) << 32) | countlo;
    }

    /*!
     * Return the number of TSC ticks since some past event.
     * \remark RDTSCP also returns the processor ID (chip and core) that the instruction was executed
     * on. The RDTSCP instruction is not a serializing instruction. It does wait until all previous
     * instructions have executed and all previous loads are globally visible, but it does not wait for
     * previous stores to be globally visible, and subsequent instructions may begin execution before
     * the read operation is performed.
     */
    static ALWAYS_INLINE uint64_t _get_tsc_ticks_since_reset_p(int &chip, int &core) noexcept {
        uint32_t countlo, counthi;
        uint32_t chx; // Set to processor signature register - set to chip/socket & core ID by recent Linux kernels.
        
        __asm__ volatile("RDTSCP" : "=a" (countlo), "=d" (counthi), "=c" (chx));
        chip = (chx & 0xFFF000)>>12;
        core = (chx & 0xFFF);
        return (uint64_t(counthi) << 32) | countlo;
    }
    
    /*!
     * Return the number of TSC ticks since some past event. This version is fenced.
     * \remark RDTSCP also returns the processor ID (chip and core) that the instruction was executed
     * on.
     */
    static ALWAYS_INLINE uint64_t _get_tsc_ticks_since_reset_p_fenced(int &chip, int &core) noexcept {
        uint32_t countlo, counthi;
        uint32_t chx; //Contents of processor signature register - set to chip/socket & core ID by recent Linux kernels.

        __asm__ volatile("RDTSCP;lfence" : "=a" (countlo), "=d" (counthi), "=c" (chx));
        // Notes:
        // If software requires RDTSCP to be executed only after all previous stores are globally visible, it can execute MFENCE immediately before RDTSCP.
        // If software requires RDTSCP to be executed prior to execution of any subsequent instruction (including any memory accesses), it can execute LFENCE immediately after RDTSCP.
        
        chip = (chx & 0xFFF000)>>12;
        core = (chx & 0xFFF);
        return (uint64_t(counthi) << 32) | countlo;
    }

    /*!
     * Return the number of seconds since some past event. Based on gettimeofday().
     * \remark Takes 130000ns on TC's EC2! Local performance = 30ns.
     */
    static ALWAYS_INLINE double _get_tod_seconds_since_epoch() noexcept {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec + tv.tv_usec*0.000001;
    }
    
private:
    static double seconds_per_tick_;//!< Number of seconds per TSC tick. Updated in sync_tsc_time().
    static double init_time_;//!< Reference time in seconds from gettimeofday().
    static uint64_t init_tick_;//!< Reference TSC tick.
    
    static int chip_;//!< Chip number on which sync_tsc_time() last executed.
    static int core_;//!< Core number on which sync_tsc_time() last executed.
};

// ToDo: Start using C++17 inline static initialisation e.g. static inline double seconds_per_tick_ = 0.0; //at definition.
int TCTimer::chip_=0;
int TCTimer::core_=0;

double TCTimer::seconds_per_tick_=0.0;
double TCTimer::init_time_=0.0;
uint64_t TCTimer::init_tick_=0;


#endif
