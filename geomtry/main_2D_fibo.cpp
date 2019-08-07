#include "../defines/tc_defines.h"

#include "../platform_info/platform_info.h"
#include "../sdl/sdl.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

struct Vec2 {
    double x, y;
    Vec2(const double _x, const double _y) : x(_x), y(_y) {}
};

void draw_points(const std::vector<Vec2> &vectors) {
    for (const auto &vect : vectors) {
        sdl::render_dot(vect.x + 400, vect.y + 300, 0, 0, 0);
    }
}

std::vector<Vec2> fibonacci_spiral_disc(const int num_points, const double k) {
    std::vector<Vec2> vectors;
    vectors.reserve(num_points);

    const double gr=(sqrt(5.0) + 1.0) / 2.0;  // golden ratio = 1.6180339887498948482
    const double ga=(2.0 - gr) * (2.0*M_PI);  // golden angle = 2.39996322972865332

    for (size_t i=1; i <= num_points; ++i) {
        const double r = sqrt(i) * k;
        const double theta = ga * i;

        const double x = cos(theta) * r;
        const double y = sin(theta) * r;

        vectors.emplace_back(x, y);
    }
    
    return vectors;
}


int main()
{
    DBN(platform_info::get_cpu_brand_string())
    DBN(platform_info::get_compiler())
    DBN(platform_info::is_intel_cpu())
    DBN(platform_info::is_drng_supported())
    
    sdl::init(800, 600);
    
    bool quits = false;
    SDL_Event es;
    
    while( !quits ) {
        sdl::clear_buffer();
        
        const auto points = fibonacci_spiral_disc(500, 10.0);
        draw_points(points);
        
        sdl::update_display();
        
        while( SDL_PollEvent( &es ) != 0 ) {
            if( es.type == SDL_QUIT ) quits = true;
        }
        
        SDL_Delay(10);
    }
    
    sdl::quit();

    return 0;
}
