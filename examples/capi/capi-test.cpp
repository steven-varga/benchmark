#include <iostream>
#include <vector>
#include <algorithm>
#include <h5bench/all>
#include <h5cpp/core>
#include <h5cpp/io>
#include <fmt/core.h>
#include <fstream>

namespace bh = h5::bench;

bh::arg_x record_size{10'000, 100'000, 1'000'000, 10'000'000};
bh::warmup warmup{3};
bh::sample sample{10};
h5::dcpl_t chunk_size = h5::chunk{4096};
using fixed_length_string_t = char[40];


int main(int argc, const char **argv){
  size_t max_size = *std::max_element(record_size.begin(), record_size.end());

  h5::fd_t fd = h5::create("h5cpp.h5", H5F_ACC_TRUNC);
  auto strings = h5::utils::get_test_data<std::string>(max_size, 10, sizeof(fixed_length_string_t));
	std::vector<char[sizeof(fixed_length_string_t)]> data(strings.size());
		for (size_t i = 0; i < data.size(); i++)
			strncpy(data[i], strings[i].data(), sizeof(fixed_length_string_t));
    
  // set the transfer size for each batch
  std::vector<size_t> transfer_size;
  for (auto i : record_size)
      transfer_size.push_back(i * sizeof(fixed_length_string_t));
  
  //use H5CPP  modify VL type to fixed length
  h5::dt_t<fixed_length_string_t> dt{H5Tcreate(H5T_STRING, sizeof(fixed_length_string_t))};
  H5Tset_cset(dt, H5T_CSET_UTF8);

  std::vector<h5::ds_t> ds;
  // create separate dataset for each batch
  for(size_t i=0; i < record_size.rank; i++){
      hsize_t size = record_size[i];
      auto name = fmt::format("fixed length string CAPI-{:010d}", size);
      // function return value, a hid_t descriptor is wrapped with RAII enabled descriptor 
      h5::sp_t file_space{H5Screate_simple(1, &size, nullptr)};
      ds.push_back(
        h5::ds_t{ // <- enable RAII by wrapping the returned hid_t in the right H5CPP descriptor
          // H5CPP handles can be passed to CAPI calls directly
          H5Dcreate2(fd, name.data(), dt, file_space, h5::default_lcpl, chunk_size, h5::default_dapl)
      });
  } 
    
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

