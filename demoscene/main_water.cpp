// Water Flow (500 FPS @ 256x256 on Broadwell 2.9GHz i5)

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
#include <string>



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
    
    const int texture_width = 320;
    const int texture_height = 320;
    sdl::init_pixel_texture(texture_width, texture_height);
    
    int **buffers = new int *[2];
    buffers[0] = new int[texture_width * texture_height];
    buffers[1] = new int[texture_width * texture_height];
    int previous_buf_idx = 0, current_buf_idx = 1;

    memset(buffers[0], 0, texture_width * texture_height * sizeof(int));
    memset(buffers[1], 0, texture_width * texture_height * sizeof(int));

    // Generate a black to bright blue water colour paletter.
    sdl::set_colour_palette(
                            {
                                {0, 0,0,0}, //black
                                {255, 0,200,255} //bright blue
                            });
    
    bool quit = false;
    SDL_Event e;
    
    int tx = texture_width >> 1;
    int ty = texture_height >> 1;
    
    const double start_time = TCTimer::get_time();
    int num_frames = 0;
    
    while ((!quit)) {// && (num_frames++ < 3000)) {
        
        if (rng.next_float() < 0.25)
        { // Add a water drop.
            uint8_t rc = rng.next(256); // drop element volume.
            
            const int x = rng.next(2, texture_width-4); // drop position.
            const int y = rng.next(2, texture_height-4);
            const int y_offset = y * texture_width;
            
            // Cross shaped water drop.
            buffers[previous_buf_idx][x + y_offset] = rc;
            buffers[previous_buf_idx][x+1 + y_offset] = rc;
            buffers[previous_buf_idx][x-1 + y_offset] = rc;
            buffers[previous_buf_idx][x + y_offset+texture_width] = rc;
            buffers[previous_buf_idx][x + y_offset-texture_width] = rc;
        }

        { // Add some water around mouse position.
            uint8_t rc = 255;
            
            if ((ty>2) && (ty<(texture_height-2)))
                for (int y=ty-1; y<=ty+1; ++y) {
                    const int y_offset = y * texture_width;
                    
                    for (int x=tx-1; x<=tx+1; ++x) {
                        buffers[previous_buf_idx][x + y_offset] = rc;
                    }
                }
        }
        
        { // Water flow - Very cool how this works!
            for (int y=1; y<(texture_height-1); ++y) {
                const int y_offset = y * texture_width;
                
                for (int x=1; x<texture_width-1; ++x) {
                    buffers[current_buf_idx][x + y_offset] =
                    // Surrounding water will affect water height map ...
                    ((buffers[previous_buf_idx][x + y_offset-texture_width] +
                      buffers[previous_buf_idx][x-1 + y_offset] +
                      buffers[previous_buf_idx][x+1 + y_offset] +
                      buffers[previous_buf_idx][x + y_offset+texture_width]) >> 1) -
                    // positively or negatively depending on current height!
                    buffers[current_buf_idx][x + y_offset];
                    
                    // Apply small extinction to water waves.
                    buffers[current_buf_idx][x + y_offset] -= buffers[current_buf_idx][x + y_offset] >> 5;
                }
            }
        }
        
        { // Move the water to the SDL pixel buffer using the water colour palette.
            for (int y=0; y<texture_height; ++y)
            {
                const int y_offset = y * texture_width;
                
                for (int x=0; x<texture_width; ++x)
                {
                    int water_height = buffers[current_buf_idx][x + y_offset]+128;
                    // Note: For refraction effect, use the water height map to calculate and offset texture lookup.
                    
                    if (water_height > 255) water_height = 255;
                    else if (water_height < 0) water_height = 0;
                    
                    sdl::put_pixel(x + y_offset, sdl::palette_[water_height]);
                }
            }
        }
        
        sdl::update_display();
        std::swap(previous_buf_idx, current_buf_idx);
        
        while( SDL_PollEvent( &e ) != 0 )
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            } else
                if (e.type == SDL_MOUSEMOTION)
                {
                    const int sx = e.motion.x;
                    const int sy = e.motion.y;
                    tx = sx * texture_width / screen_width;
                    ty = sy * texture_height / screen_height;
                }
        }
        
        SDL_Delay(250);
    }
    
    const int FPS = num_frames / (TCTimer::get_time() - start_time);
    DBN(FPS)
    
    sdl::quit();
    
    delete [] buffers[0];
    delete [] buffers[1];
    delete [] buffers;
    
    return 0;
}
