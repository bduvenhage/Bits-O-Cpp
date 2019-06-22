#ifndef TC_DEFINES_H
#define TC_DEFINES_H 1

#include <iostream>

#define ALWAYS_INLINE inline __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))

#ifdef TCDEBUG
#define BBBD(a) if (a) {std::cerr << "BBBoomD " << #a << " @ " << __FILE__ << " line "<< __LINE__ << "!\n"; std::cerr.flush(); exit(-1);}
#else
#define BBBD(a)
#endif

#define DBHEX std::cerr << std::hex << std::uppercase << std::showbase;
#define DBDEC std::cerr << std::dec << std::nouppercase << std::noshowbase;
#define DBS(a) std::cerr << #a << " = " << (a) << "  ";
#define DBN(a) std::cerr << #a << " = " << (a) << std::endl;

#endif //TC_DEFINES_H
