// Flow Fire (900 FPS @ 256x256 on Broadwell 2.9GHz i5)

#include "../defines/tc_defines.h"

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
    DBN(platform_info::get_cpu_brand_string())
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
    
    int *buffer = new int[texture_width * texture_height];
    
    // Generate a black,orange,yellow,white colour paletter.
    sdl::set_colour_palette(
                            {
                                {0, 0,0,0}, //black
                                {60, 255,128,0}, //orange
                                {130, 255,255,0}, //yellow
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
            const int fire_cell_size = 2;
            
            int y_offset = (texture_width-fire_cell_size) * texture_width;
            
            for (int x=0; x<texture_width; )
            { // First row of randomness.
                const uint8_t rc = rng.next(256);
                for (int i=0; i<fire_cell_size; ++i, ++x) {
                    buffer[x + y_offset] = rc;
                }
            }
            
            for (int j=1; j<fire_cell_size; ++j)
            { // Duplicate random row a couple of times.
                y_offset += texture_width;
                
                for (int x=0; x<texture_width; ++x) {
                    buffer[x + y_offset] = buffer[x + y_offset - texture_width];
                }
            }
        }
        
        { // Add some fire around mouse position.
            uint8_t rc = rng.next(128) + 128;
            
            if ((ty>0) && (ty<(texture_height-1)))
                for (int y=ty-1; y<=ty+1; ++y) {
                    const int y_offset = y * texture_width;
                    
                    for (int x=tx-1; x<=tx+1; ++x) {
                        buffer[x + y_offset] = rc;
                    }
                }
        }
        
        { // Fire flow.
            for (int y=0; y<(texture_height-2); ++y) {
                const int y_offset = y * texture_width;
                
                for (int x=0; x<texture_width; ++x) {
                    buffer[x + y * texture_width] = (buffer[x+0 + y_offset] +
                                                     buffer[x+0 + y_offset + texture_width] +
                                                     buffer[x+0 + y_offset + texture_width] +
                                                     buffer[x+1 + y_offset + texture_width]) * 0.245;
                }
            }
        }
        
        { // Move the fire to the SDL pixel buffer using the fire colour palette.
            for (int y=0; y<(texture_height-2); ++y)
            {
                const int y_offset = y * texture_width;
                
                for (int x=0; x<texture_width; ++x)
                {
                    sdl::put_pixel(x + y_offset, sdl::palette_[buffer[x + y_offset]]);
                }
            }
        }
        
        sdl::update_display();
        
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
        
        SDL_Delay(25);
    }
    
    const int FPS = num_frames / (TCTimer::get_time() - start_time);
    DBN(FPS)
    
    sdl::quit();
    delete [] buffer;
    
    return 0;
}
