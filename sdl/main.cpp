#define ALWAYS_INLINE inline __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))

#ifdef TCDEBUG
#define BBBD(a) if (a) {std::cerr << "BBBoomD " << #a << " @ " << __FILE__ << " line "<< __LINE__ << "!\n"; std::cerr.flush(); exit(-1);}
#else
#define BBBD(a)
#endif

#define DBS(a) std::cerr << #a << " = " << (a) << "  ";
#define DBN(a) std::cerr << #a << " = " << (a) << std::endl;

#include "../math/tc_math.h"
#include "../sdl/sdl.h"

#include <iostream>
#include <iomanip>

int main()
{
    sdl::init(800, 600);
    
    std::cerr << std::setprecision(20);
    double x=-5.0;
    double ex = exp(x);
    double fex = tc_math::fast_exp_64(x);
    
    const double yoff = 10.0;
    const double yscale = 40.0;
    
    sdl::render_line(0, yoff, 799, yoff, 255, 255, 255);
    sdl::render_line(399, 0, 399, 599, 255, 255, 255);
    
    for (int i=1; i<800; ++i)
    {
        const double nx = x + 10.0/800;
        const double nex = exp(nx);
        const double nfex = tc_math::fast_exp_64(nx);
        
        sdl::render_line(i-1, ex*yscale+yoff, i, nex*yscale+yoff, 255, 0, 0);
        sdl::render_line(i-1, fex*yscale+yoff, i, nfex*yscale+yoff, 0, 255, 0);
        
        x=nx;
        ex=nex;
        fex=nfex;
    }
    
    sdl::update_display();
    
    bool quits = false;
    SDL_Event es;
    
    while( !quits )
    {
        while( SDL_PollEvent( &es ) != 0 )
        {
            if( es.type == SDL_QUIT )
            {
                quits = true;
            }
            
            SDL_Delay(10);
        }
    }
    
    sdl::quit();
}
