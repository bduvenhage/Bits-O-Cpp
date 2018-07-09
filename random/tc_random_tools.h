/*
 const uint32_t rng_seed_ = 0;
 TCRandom<TC_MCG_Lehmer_RandFunc32> rng_(rng_seed_);
 
 //===================================//
 //=== Measure performance of RNGs ===//
 //===================================//
 TCTimer::init_timer(2.89992e+09);
 
 const double start_time = TCTimer::get_tsc_time();
 
 const uint64_t num_iterations = uint64_t(1)<<32;
 uint64_t t=0,f=0;
 uint32_t ri = 0;
 double rf = 0.0;
 
 for (uint64_t i=0; i <= num_iterations; ++i)
 {
 ri += rng_.next();
 //ri = rng_.next();
 //rf += rng_.next_double();
 //t+=ri&1;
 //f+=!(ri&1);
 }
 
 const double end_time = TCTimer::get_tsc_time();
 
 DBN(ri)
 DBN(rf/num_iterations)
 DBN(t/double(f))
 DBN((end_time-start_time) * 1000000000.0 / num_iterations)
 
 exit(0);
 //===================================//
 //===================================//
 //===================================//
 */

/*
 //===========================//
 //=== PractRand generator ===//
 //===========================//
 //http://www.pcg-random.org/posts/how-to-test-with-practrand.html
 //PractRand_0.93 $ ../TCO2018_R2/xcode/Release/main | ./RNG_test stdin
 
 //freopen(NULL, "wb", stdout);  // Only necessary on Windows, but harmless.
 
 const uint32_t rng_seed_ = 123456789;
 TCRandom<TC_LCG_RandFunc> rng_(rng_seed_);
 //TCRandom<TC_XOR_SHIFT_128_RandFunc> rng_(rng_seed_);
 //TCRandom<TC_XOR_SHIFT_96_RandFunc> rng_(rng_seed_);
 //TCRandom<TC_PCG32_RandFunc> rng_(rng_seed_);
 //TCRandom<TC_MT32_RandFunc> rng_(rng_seed_);//
 //TCRandom<CPP_MT32_RandFunc> rng_(rng_seed_);
 
 while (1)
 {
 const uint32_t value = rng_.next();
 fwrite((void*) &value, (rng_.num_bits() >> 3), 1, stdout);
 }
 
 exit(0);
 //===========================//
 //===========================//
 //===========================//
 */


/*
 //=====================================================//
 //=== Visualise bias when rejection not compiled in ===//
 //=====================================================//
 sdl::init(800, 600);
 
 const uint32_t rng_seed_ = 123456789;
 
 TCRandom<TC_LCG_RandFunc> rng_(rng_seed_);
 const uint32_t s = (uint32_t(1)<<15)-1000;
 //OR
 //TCRandom<TC_PCG32_RandFunc> rng_(rng_seed_);
 //const uint32_t s = (uint32_t(1)<<31)-1000000;
 
 const uint32_t buffer_size = std::min(s, uint32_t(1000000));
 
 uint16_t numbers[buffer_size];
 for (int64_t i=0; i<buffer_size; ++i) numbers[i] = 0;
 
 for (int64_t i=0; i<(int64_t(300)*s); ++i)
 {
 uint32_t r = rng_.next(s);
 if (r<buffer_size) numbers[r]+=1;
 }
 
 for (uint32_t i=0; i<buffer_size; ++i)
 {
 sdl::render_point((i+0.5)*800.0/double(buffer_size), 599 - numbers[i]*0.1, 255, 255, 255);
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
 
 exit(0);
 //=====================================================//
 //=====================================================//
 //=====================================================//
 */


