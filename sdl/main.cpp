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
#include <vector>


using Vertex = std::pair<int, int>;
using Line = std::pair<Vertex, Vertex>;

std::vector<Line> grow_koch_snowflake(const std::vector<Line> &input_snowflake)
{
    std::vector<Line> output_snowflake;
    output_snowflake.reserve(input_snowflake.size() * 4);
    
    for (const auto &line : input_snowflake)
    {
        const double x0 = line.first.first + 0.5;
        const double x1 = line.second.first + 0.5;

        const double y0 = line.first.second + 0.5;
        const double y1 = line.second.second + 0.5;

        const double xa = (x1-x0) * (1.0/3.0) + x0;
        const double xb = (x1-x0) * (1.0/2.0) + x0 - sin(60.0*M_PI/180.0)*(y1-y0)/3.0;
        const double xc = (x1-x0) * (2.0/3.0) + x0;
        
        const double ya = (y1-y0) * (1.0/3.0) + y0;
        const double yb = (y1-y0) * (1.0/2.0) + y0 + sin(60.0*M_PI/180.0)*(x1-x0)/3.0;
        const double yc = (y1-y0) * (2.0/3.0) + y0;
        
        output_snowflake.push_back(Line(Vertex(x0, y0), Vertex(xa, ya)));
        output_snowflake.push_back(Line(Vertex(xa, ya), Vertex(xb, yb)));
        output_snowflake.push_back(Line(Vertex(xb, yb), Vertex(xc, yc)));
        output_snowflake.push_back(Line(Vertex(xc, yc), Vertex(x1, y1)));
    }
    
    return output_snowflake;
}

void draw_snowflake(const std::vector<Line> &input_snowflake)
{
    for (const auto &line : input_snowflake)
    {
        sdl::render_line(line.first.first, line.first.second,
                         line.second.first, line.second.second,
                         0, 0, 0);
    }
}


int main()
{
    // Init the SDL system.
    sdl::init(800, 600);
    
    // === Create a snowflake ===
    const int snowflake_depth = 5;

    const int num_sides = 3;
    std::vector<Line> snowflake;

    for (int i=0; i<num_sides; ++i)
    {
        const int x0 = 250 * cos(-((i+0)*2.0*M_PI)/num_sides) + 400;
        const int y0 = 250 * sin(-((i+0)*2.0*M_PI)/num_sides) + 300;
        const int x1 = 250 * cos(-((i+1)*2.0*M_PI)/num_sides) + 400;
        const int y1 = 250 * sin(-((i+1)*2.0*M_PI)/num_sides) + 300;
        
        snowflake.push_back(Line(Vertex(x0, y0), Vertex(x1, y1)));
    }
    
    for (int i=0; i<snowflake_depth; ++i)
    {
        snowflake = grow_koch_snowflake(snowflake);
    }
    // ==========================
    
    // === SDL event loop ===
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
            
            sdl::clear_buffer();

            // Draw the snowflake using lines.
            draw_snowflake(snowflake);
            
            // Update the display with the snowflake
            sdl::update_display();

            SDL_Delay(10);
        }
    }
    
    sdl::quit();
}
