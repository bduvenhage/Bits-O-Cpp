#include "../defines/tc_defines.h"

#include "../platform_info/platform_info.h"
#include "../sdl/sdl.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

struct Vec3 {
    double x, y, z;
    
    Vec3() : x(0.0), y(0.0), z(0.0) {}
    Vec3(const double _x, const double _y, const double _z) : x(_x), y(_y), z(_z) {}
    
    Vec3 operator+(const Vec3 &v) const {
        return Vec3(x+v.x, y+v.y, z+v.z);
    }
    
    Vec3 &operator+=(const Vec3 &v) {
        x+=v.x; y+=v.y; z+=v.z;
        return *this;
    }

    Vec3 operator-(const Vec3 &v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 operator*(const double &s) const {
        return Vec3(x*s, y*s, z*s);
    }

    Vec3 operator/(const double &s) const {
        return Vec3(x/s, y/s, z/s);
    }

    Vec3 normalised() const {
        const double r = sqrt(x*x + y*y + z*z);
        return Vec3(x/r, y/r, z/r);
    }
    
    static double calc_dist_sq(const Vec3 &a, const Vec3 &b) {
        return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z);
    }
    
    static double dot(const Vec3 &a, const Vec3 &b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }
};

void draw_points(const std::vector<Vec3> &vectors) {
    for (const auto &vect : vectors) {
        if (vect.z > 0.0) { // Only draw front facing points.
            sdl::render_dot(vect.x*150.0+400, vect.y*150.0+300, 0, 0, 0);
        }
    }
}

template<class T>
ALWAYS_INLINE const T& clamp(const T& d, const T& l, const T& u) {
    const T& t = d < l ? l : d;
    return t > u ? u : t;
}

std::vector<Vec3> fibonacci_spiral_sphere(const int num_points) {
    std::vector<Vec3> vectors;
    vectors.reserve(num_points);
    
    const double gr=(sqrt(5.0) + 1.0) / 2.0;  // golden ratio = 1.6180339887498948482
    const double ga=(2.0 - gr) * (2.0*M_PI);  // golden angle = 2.39996322972865332

    for (size_t i=1; i <= num_points; ++i) {
        const double lat = asin(-1.0 + 2.0 * double(i) / (num_points+1));
        const double lon = ga * i;

        const double x = cos(lon)*cos(lat);
        const double y = sin(lon)*cos(lat);
        const double z = sin(lat);

        vectors.emplace_back(x, y, z);
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
        
        const auto points = fibonacci_spiral_sphere(2500);
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
