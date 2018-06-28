
//! Approximate exp - Based on Schraudolph 1999 - Valid for x in approx range (-700, 700)
ALWAYS_INLINE double fast_exp(const double x) noexcept
{
    union{double d_; int32_t i_[2];} uid;
    //BBBD(sizeof(uid)!=8)
    uid.i_[0] = 0;
    uid.i_[1] = int32_t(double((1<<20) / log(2.0)) * x + double(1023 * (1<<20) - 0));//params for zero at zero.
    return uid.d_;
}

const double T = 50.0 * (1.000001 - pow(time / max_time_, 1.0));
const double p = exp((solution_vec[si].length_ - new_solution.length_)/T);

if (rng_.next_double() < p)
//if (new_solution.length_ < solution_vec[si].length_)
{

}


//Plot fast_exp...
int main()
{
    sdl::init(800, 600);
    
    std::cerr << std::setprecision(30);
    DBN(1.0/fast_exp(0.0))
    DBN(1.0/exp(0.0))
    
    DBN(fast_exp(-700.0))
    DBN(exp(-700.0))
    
    double x=-5.0;
    double ex = exp(x);
    double fex = fast_exp(x);
    
    const double yoff = 10.0;
    const double yscale = 40.0;
    
    sdl::render_line(0, yoff, 799, yoff, 255, 255, 255);
    sdl::render_line(399, 0, 399, 599, 255, 255, 255);
    
    for (int i=1; i<800; ++i)
    {
        const double nx = x + 10.0/800;
        const double nex = exp(nx);
        const double nfex = fast_exp(nx);
        
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
