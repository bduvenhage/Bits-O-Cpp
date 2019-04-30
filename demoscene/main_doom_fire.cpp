// DOOM Fire (850 FPS on Broadwell 2.9GHz i5) - credit to http://fabiensanglard.net/doom_fire_psx/ for a recent description of the technique.

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
#include "../platform_info/platform_info.h"

#include "../sdl/sdl.h"

#include <map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>



int main(void)
{
    DBN(platform_info::get_cpu())
    DBN(platform_info::get_compiler())
    
    const double EXP_TSC_FREQ = 2.89992e+09;
    TCTimer::init_timer(EXP_TSC_FREQ);
    TCRandom<TC_MCG_Lehmer_RandFunc32> rng(987654321); // Fast random number generator used to fill set.
    
    const int screen_width = 640;
    const int screen_height = 640;
    sdl::init(screen_width, screen_height);
    
    const int texture_width = 256;
    const int texture_height = 256;
    sdl::init_pixel_texture(texture_width, texture_height);
    
    int *buffer = new int[texture_width * texture_height];
    
    // Generate a black,orange,yellow,white colour paletter.
    sdl::set_colour_palette(
                            {
                                {0, 0,0,0}, //black
                                {120, 255,128,0}, //orange
                                {200, 255,255,0}, //yellow
                                {255, 255,255,255} //white
                            });
    
    bool quit = false;
    SDL_Event e;
    
    int tx = texture_width >> 1;
    int ty = texture_height >> 1;
    
    const double start_time = TCTimer::get_time();
    int num_frames = 0;
    
    while ((!quit)) {// && (num_frames++ < 3000)) {
        { // Add some heat at bottom.
            for (int y=(texture_height-3); y<texture_height; ++y) {
                const int y_offset = y * texture_width;
                
                for (int x=0; x<texture_width; ++x) {
                    buffer[x + y_offset] = 255;
                }
            }
        }
        
        { // Add some heat around mouse position.
            if ((ty>0) && (ty<(texture_height-1)))
                for (int y=ty-1; y<=ty+1; ++y) {
                    const int y_offset = y * texture_width;
                    
                    for (int x=tx-1; x<=tx+1; ++x) {
                        buffer[x + y_offset] = 180;
                    }
                }
        }
        
        { // Fire flow.
            for (int y=1; y<(texture_height-1); ++y) {
                const int y_offset = y * texture_width;
                
                for (int x=0; x<texture_width; ++x) {
                    const int dst_x = x - rng.next(3) + 1; //controls fire scatter.
                    const int v = buffer[x + y_offset] - rng.next(4); //rng controls extinction.
                    buffer[dst_x + y_offset - texture_width] = (v>=0) ? v : 0;
                }
            }
        }
        
        { // Move the fire to the SDL pixel buffer using the fire colour palette.
            for (int y=0; y<texture_height; ++y) {
                const int y_offset = y * texture_width;
                
                for (int x=0; x<texture_width; ++x) {
                    sdl::put_pixel(x + y_offset, sdl::palette_[buffer[x + y_offset]]);
                }
            }
        }
        
        sdl::update_display();
        
        while( SDL_PollEvent( &e ) != 0 ) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else
                if (e.type == SDL_MOUSEMOTION) {
                    const int sx = e.motion.x;
                    const int sy = e.motion.y;
                    tx = sx * texture_width / screen_width;
                    ty = sy * texture_height / screen_height;
                }
        }
        
        SDL_Delay(25);
    }
    
    const int FPS = num_frames / (TCTimer::get_time() - start_time);
    DBN(FPS)
    
    sdl::quit();
    delete [] buffer;
    
    return 0;
}
