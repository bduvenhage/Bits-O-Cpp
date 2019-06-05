# Bits-O-Cpp Demoscene fire
See post https://bduvenhage.me/memory_bank/2019/04/30/memory-bank_making-fire-and-water.html

To run, first install SDL. On OSX I could just do `brew install sdl2`.

Then clone the repo and do:
```console
cd Bits-O-Cpp/demoscene
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make
./main_fire
./main_doom_fire
./main_water
```
