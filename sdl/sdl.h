//================================//
//=== Simple DirectMedia Layer ===//
//================================//
#include "../defines/tc_defines.h"

#include <SDL2/SDL.h>
#include <vector>

namespace sdl {
    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;

    SDL_Texture *pixel_texture_ = nullptr;
    uint32_t *pixels_ = nullptr;
    int pixel_texture_width_ = 0;
    uint32_t *palette_;
    
    
    bool init(const int screen_width, const int screen_height) {
        bool success = true;
        
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
                printf("Warning: Linear texture filtering not enabled!");
            }
            
            window_ = SDL_CreateWindow("SDL View", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       screen_width, screen_height, 0);
            if(window_ == nullptr) {
                printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
                
                if(renderer_ == nullptr) {
                    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
                else {
                    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
                }
            }
        }
        
        return success;
    }
    
    bool init_colour_palette() {
        if (palette_ != nullptr) delete [] palette_;
        palette_ = new uint32_t[256];
        
        return true;
    }

    void set_default_colour_palette() {
        init_colour_palette();
        
        // Set a grayscale default palette.
        for (int i=0; i<256; ++i) {
            palette_[i] = (i << 16) | (i << 8) | i;
        }
        
    }

    struct ColourPoint {
        ColourPoint(int index, uint8_t r, uint8_t g, uint8_t b) : index_(index), r_(r), g_(g), b_(b)
        {}
        
        int index_;
        uint8_t r_, g_, b_;
    };
    
    bool set_colour_palette(const std::vector<ColourPoint> &colour_point_vec) {
        init_colour_palette();
        
        const int num_colour_points = colour_point_vec.size();
        
        if (num_colour_points < 2) return false;
        
        for (int p=0; p<(num_colour_points-1); ++p) {
            ColourPoint ca = colour_point_vec[p];
            ColourPoint cb = colour_point_vec[p+1];
            
            if (ca.index_ > cb.index_) std::swap(ca, cb);
            
            for (int i=ca.index_; i<=cb.index_; ++i) {
                double s = (i-ca.index_)/double(cb.index_-ca.index_);
                int r = (cb.r_ - ca.r_) * s + ca.r_ + 0.5;
                int g = (cb.g_ - ca.g_) * s + ca.g_ + 0.5;
                int b = (cb.b_ - ca.b_) * s + ca.b_ + 0.5;
                
                palette_[i] = (r << 16) + (g << 8) + b;
            }
        }
            
        return true;
    }
    
    
    bool init_pixel_texture(const int texture_width, const int texture_height) {
        if (pixels_ != nullptr) delete [] pixels_;
        pixels_ = new uint32_t[texture_width * texture_height];

        pixel_texture_width_ = texture_width;
        
        pixel_texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STATIC, texture_width, texture_height);
        
        return true;
    }
    
    void quit() {
        if (pixel_texture_ != nullptr) {
            if (pixels_!=nullptr) delete [] pixels_;
            pixels_ = nullptr;
            
            SDL_DestroyTexture(pixel_texture_);
            pixel_texture_ = nullptr;
            pixel_texture_width_ = 0;
        }
        
        if (palette_!=nullptr) delete [] palette_;
        palette_ = nullptr;

        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        renderer_ = nullptr;
        
        SDL_Quit();
    }
    
    void clear_buffer()
    {
        SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer_);
    }
    
    void update_display()
    {
        if (pixel_texture_ != nullptr) {
            SDL_UpdateTexture(pixel_texture_, NULL, pixels_, pixel_texture_width_ * sizeof(uint32_t));
            SDL_RenderCopy(renderer_, pixel_texture_, NULL, NULL);
        }
        
        SDL_RenderPresent(renderer_);
    }
    
    ALWAYS_INLINE void put_pixel(const int x, const int y,
                                 const uint8_t r, const uint8_t g, const uint8_t b)
    {
        pixels_[x + y * pixel_texture_width_] = (r << 16) + (g << 8) + b;
    }
    
    ALWAYS_INLINE void put_pixel(const int x, const int y, const uint32_t c)
    {
        pixels_[x + y * pixel_texture_width_] = c;
    }

    ALWAYS_INLINE void put_pixel(const int offset, const uint32_t c)
    {
        pixels_[offset] = c;
    }
    
    ALWAYS_INLINE void render_dot(const int x, const int y,
                    const uint8_t r, const uint8_t g, const uint8_t b,
                    const int a = 255)
    {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        SDL_Rect fillRect = {x-2, y-2, 5, 5};
        SDL_RenderFillRect(renderer_, &fillRect);
    }
    
    ALWAYS_INLINE void render_dot(const int x, const int y, const uint32_t c)
    {
        const uint8_t r = (c>>16) & 255;
        const uint8_t g = (c>>8) & 255;
        const uint8_t b = c & 255;
        const uint8_t a = 255;
        render_dot(x, y, r, g, b, a);
    }

    ALWAYS_INLINE void render_point(const int x, const int y,
                      const uint8_t r, const uint8_t g, const uint8_t b,
                      const int a = 255)
    {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        SDL_RenderDrawPoint(renderer_, x, y);
    }

    ALWAYS_INLINE void render_point(const int x, const int y, const uint32_t c)
    {
        const uint8_t r = (c>>16) & 255;
        const uint8_t g = (c>>8) & 255;
        const uint8_t b = c & 255;
        const uint8_t a = 255;
        render_point(x, y, r, g, b, a);
    }
    
    ALWAYS_INLINE void render_line(const int x0, const int y0, const int x1, const int y1,
                                   const uint8_t r, const uint8_t g, const uint8_t b,
                                   const int a = 255)
    {
        SDL_SetRenderDrawColor(renderer_, r, g, b, a);
        SDL_RenderDrawLine(renderer_, x0, y0, x1, y1);
    }
    
    ALWAYS_INLINE void render_line(const int x0, const int y0, const int x1, const int y1, const uint32_t c)
    {
        const uint8_t r = (c>>16) & 255;
        const uint8_t g = (c>>8) & 255;
        const uint8_t b = c & 255;
        const uint8_t a = 255;
        render_line(x0, y0, x1, y1, r, g, b, a);
    }
}



// === main code

//sdl::init(640, 480);


//bool quit = false;
//SDL_Event e;
//
//while( !quit )
//{
//    while( SDL_PollEvent( &e ) != 0 )
//    {
//        if( e.type == SDL_QUIT )
//        {
//            quit = true;
//        }
//
//        SDL_Delay(10);
//    }
//}
//
//sdl::quit();
