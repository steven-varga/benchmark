#  _________________________________________________________
#  Copyright (c) 2021-2021 Steven Varga, Toronto,ON Canada
#  Author: Varga, Steven <steven@vargaconsulting.ca>
#  _________________________________________________________

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
if(${CMAKE_VERSION} VERSION_GREATER "3.17.0")
	cmake_policy(SET CMP0074 NEW)
endif()
## match hdf5 versioning x.x.x.h5cpp-version
project(libh5bench-dev VERSION 1.10.4.6 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
find_package( Threads REQUIRED QUIET)

add_library(h5bench INTERFACE)
# check if the correct version of hdf5 available
set(H5CPP_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
if(WIN32)
	message(STATUS "***WARNING: h5bench is not fully tested on WINDOWS ***")
elseif(UNIX)
	set(HDF5_ROOT /usr/local/ /usr) # there maybe more upto date library in these places
	include(GNUInstallDirs)
	find_package(PkgConfig REQUIRED QUIET)
endif()
if(APPLE)
	message(STATUS "***WARNING: h5bench is not fully tested on apple ***")
endif()
if(ANDROID)
	message(STATUS "***WARNING: h5bench is not fully tested on android ***")
endif()

include(FindHDF5)
if(HDF5_VERSION VERSION_LESS ${H5CPP_VERSION})
	message(FATAL_ERROR "-- !!! H5BENCH examples require of HDF5 v${H5CPP_VERSION} or greater!!!")
else()
	message("-- H5CPP ${PROJECT_VERSION} matches with minimum required HDF5 v${HDF5_VERSION}")
endif()

if(HDF5_VERSION VERSION_GREATER 1.12.0)
	message("make sure to check out H5CPP master branch")
endif()

include(FindMPI MPI_SKIP_COMPILER_WRAPPER)
if( MPI_FOUND AND HDF5_IS_PARALLEL )
	message("-- MPI and PHDF5 found: Parallel H5CPP enabled")
endif()


find_package(GTest)
find_package(fmt)

option(H5BENCH_BUILD_TESTS "Build tests" OFF)
if(H5BENCH_BUILD_TESTS)
	message("-- gtest is disabled in the version, new test framework in progress...")
endif()


#installation
target_include_directories(h5bench INTERFACE
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)

if(WIN32)
	set(INSTALL_DIR_CMAKE "${CMAKE_INSTALL_PREFIX}/share/h5bench/cmake")
	set(INSTALL_DIR_INCLUDE  "${CMAKE_INSTALL_PREFIX}/include")
	set(INSTALL_DIR_EXAMPLES "${CMAKE_INSTALL_PREFIX}/examples")
elseif(UNIX)
	set(INSTALL_DIR_CMAKE    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/h5bench")
	set(INSTALL_DIR_INCLUDE  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
	set(INSTALL_DIR_EXAMPLES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/h5bench")
endif()

message( "-- Install directory: ${INSTALL_DIR_INCLUDE}")

# copy header only library into DESTDIR
install(DIRECTORY h5cpp DESTINATION ${INSTALL_DIR_INCLUDE})
install(TARGETS h5cpp EXPORT h5bench-targets)

install(DIRECTORY examples/capi     	  DESTINATION "${INSTALL_DIR_EXAMPLES}")
install(FILES     examples/CMakeLists.txt DESTINATION "${INSTALL_DIR_EXAMPLES}")
install(FILES     examples/Makefile       DESTINATION "${INSTALL_DIR_EXAMPLES}")

#generate cmake config and version files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/config.cmake.in
    ${CMAKE_BINARY_DIR}/h5bench-config.cmake
    INSTALL_DESTINATION ${INSTALL_DIR_CMAKE}
)
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/h5bench-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(PACKAGE h5bench)
export(EXPORT h5bench-targets)

install(EXPORT h5bench-targets DESTINATION ${INSTALL_DIR_CMAKE})
install(FILES
    ${CMAKE_BINARY_DIR}/h5bench-config.cmake
    ${CMAKE_BINARY_DIR}/h5bench-config-version.cmake
    DESTINATION ${INSTALL_DIR_CMAKE}
)

