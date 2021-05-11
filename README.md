# h5bench
an H5CPP based benchmark utility with pythonic syntax for HDF5 CAPI and H5CPP



```
// EXPERIMENT: arguments, including lambda function may be passed in arbitrary order
bh::throughput(
    bh::name{"fixed length string CAPI"}, record_size, warmup, sample,
    [&](size_t idx, size_t size_) -> double {
        hsize_t size = size_;
        // memory space
        h5::sp_t mem_space{H5Screate_simple(1, &size, nullptr )};
        H5Sselect_all(mem_space);
        // file space
        h5::sp_t file_space{H5Dget_space(ds[idx])};
        H5Sselect_all(file_space);
        // IO call
        H5Dwrite( ds[idx], dt, mem_space, file_space, H5P_DEFAULT, data.data());
        return transfer_size[idx];
});
```

From `examples/capi` execute make to test the performance

```
steven@io:~/projects/h5bench/examples/capi$ make
g++ -I/usr/local/include -I/usr/include -I../../include -o capi-test.o   -std=c++17 -Wno-attributes -c capi-test.cpp
g++ capi-test.o -lhdf5  -lz -ldl -lm -o capi-test
taskset 0x1 ./capi-test
[name                                              ][total events][Mi events/s] [ms runtime / stddev] [    MiB/s / stddev ]
fixed length string CAPI                                    10000     625.0000         0.02     0.000   24461.70     256.9
fixed length string CAPI                                   100000     122.7898         0.81     0.038    4917.70     213.3
fixed length string CAPI                                  1000000      80.4531        12.43     0.217    3218.60      56.6
fixed length string CAPI                                 10000000      79.7568       125.38     0.140    3189.80       3.6
rm capi-test.o
```