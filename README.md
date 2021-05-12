# h5bench
an H5CPP based benchmark utility with pythonic syntax for HDF5 CAPI and H5CPP



```cpp
namespace bh = h5::bench;

bh::arg_x record_size{10'000, 100'000, 1'000'000, 10'000'000};
bh::warmup warmup{3};
bh::sample sample{10};
h5::dcpl_t chunk_size = h5::chunk{4096};
using fl_string_t = char[40];


int main(int argc, const char **argv){
  size_t max_size = *std::max_element(record_size.begin(), record_size.end());

  h5::fd_t fd = h5::create("h5cpp.h5", H5F_ACC_TRUNC);
  auto strings = h5::utils::get_test_data<std::string>(max_size, 10, sizeof(fl_string_t));
	std::vector<char[sizeof(fl_string_t)]> data(strings.size());
		for (size_t i = 0; i < data.size(); i++)
			strncpy(data[i], strings[i].data(), sizeof(fl_string_t));
    
  // set the transfer size for each batch
  std::vector<size_t> transfer_size;
  for (auto i : record_size)
      transfer_size.push_back(i * sizeof(fl_string_t));
  
  //use H5CPP  modify VL type to fixed length
  h5::dt_t<fl_string_t> dt{H5Tcreate(H5T_STRING, sizeof(fl_string_t))};
  H5Tset_cset(dt, H5T_CSET_UTF8);

  std::vector<h5::ds_t> ds;
  // create separate dataset for each batch
  for(auto size: record_size) ds.push_back(
    h5::create<fl_string_t>(fd, fmt::format("fixe length string CAPI-{:010d}", size), 
    chunk_size, h5::current_dims{size}, dt));

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
}

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