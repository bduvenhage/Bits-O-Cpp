//================================//
//=== Simple DirectMedia Layer ===//
//================================//
#ifdef TCSDL
#include <SDL2/SDL.h>

namespace sdl
{
    SDL_Window* window_ = NULL;
    SDL_Renderer* renderer_ = NULL;
    
    bool init(const int screen_width, const int screen_height)
    {
        bool success = true;
        
        if(SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1"))
            {
                printf("Warning: Linear texture filtering not enabled!");
            }
            
            window_ = SDL_CreateWindow("SDL View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       screen_width, screen_height, SDL_WINDOW_SHOWN);
            if(window_ == NULL)
            {
                printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
                
                if(renderer_ == NULL)
                {
                    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
                }
            }
        }
        
        return success;
    }
    
    void quit()
    {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        window_ = NULL;
        renderer_ = NULL;
        SDL_Quit();
    }
    
    void clear_buffer()
    {
        SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer_);
    }
    
    void update_display()
    {
        SDL_RenderPresent(renderer_);
    }
    
    void render_dot(const int x, const int y,
                    const uint8_t r, const uint8_t g, const uint8_t b,
                    const int a = 255)
    {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        SDL_Rect fillRect = {x-2, y-2, 5, 5};
        SDL_RenderFillRect(renderer_, &fillRect);
    }
    
    void render_point(const int x, const int y,
                      const uint8_t r, const uint8_t g, const uint8_t b,
                      const int a = 255)
    {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        SDL_RenderDrawPoint(renderer_, x, y);
    }
    
    void render_line(const int x0, const int y0, const int x1, const int y1,
                     const uint8_t r, const uint8_t g, const uint8_t b,
                     const int a = 255)
    {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        SDL_RenderDrawLine(renderer_, x0, y0, x1, y1);
    }
}
#endif


#ifdef TCSDL
bool quit = false;
SDL_Event e;

while( !quit )
{
    while( SDL_PollEvent( &e ) != 0 )
    {
        if( e.type == SDL_QUIT )
        {
            quit = true;
        }
        
        SDL_Delay(10);
    }
}

sdl::quit();
#endif
