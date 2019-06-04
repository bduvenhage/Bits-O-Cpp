# Fast exp and log2 test
See post https://bduvenhage.me/performance/machine_learning/2019/06/04/fast-exp.html

To compile:

```console
cd Bits-O-Cpp/math
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make
```

Then run either:

```console
./test_fast_exp
```

or:

```console
./test_fast_intlog2
```
