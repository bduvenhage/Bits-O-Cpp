# Bits-O-Cpp Container test
See posts https://bduvenhage.me/performance/2019/04/12/performance-of-deque.html and https://bduvenhage.me/performance/2019/04/22/size-of-hash-table.html

To compile:

```console
cd Bits-O-Cpp/containers
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make
```

Then run either:

```console
./main_vector_vs_deque
```

or:

```console
./hash_table
```
