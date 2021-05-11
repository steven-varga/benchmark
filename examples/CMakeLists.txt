#  _________________________________________________________
#  Copyright (c) 2021-2021 Steven Varga, Toronto,ON Canada
#  Author: Varga, Steven <steven@vargaconsulting.ca>
#  _________________________________________________________
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

## match hdf5 versioning x.x.x.h5cpp-version
project(h5bench-examples VERSION 1.10.4.5 LANGUAGES CXX C)

# check if the correct version of hdf5 available
set(H5BENCH_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set(HDF5_FIND_DEBUG FALSE)
include(FindHDF5)
if(HDF5_VERSION VERSION_LESS ${H5CPP_VERSION})
	message(FATAL_ERROR "H5BENCH examples require of HDF5 v${H5CPP_VERSION} or greater!!!")
else()
	message("-- H5BENCH ${PROJECT_VERSION} matches with minimum required HDF5 v${HDF5_VERSION}")
endif()
if(HDF5_VERSION VERSION_GREATER 1.12.0)
	message("make sure to use H5CPP master branch latest...")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/"
	"${CMAKE_SOURCE_DIR}/cmake/eigen3"  "${CMAKE_SOURCE_DIR}/cmake/dlib" )
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
	message(STATUS "***WARNING: h5bench is not fully tested on windows, but know to work ***")
endif()
if(APPLE)
	message(STATUS "***WARNING: h5bench is not fully tested on apple ***")
endif()

include(GNUInstallDirs)
find_package(Doxygen QUIET)

list(APPEND CMAKE_INCLUDE_PATH "/usr/include")
list(APPEND CMAKE_INCLUDE_PATH "/usr/local/include" )

#find_package(HDF5 1.10.4 REQUIRED QUIET)
include(FindHDF5)
if(HDF5_VERSION VERSION_LESS 1.10.4)
	message(FATAL_ERROR "H5BENCH examples require HDF5 minimum version is 1.10.4")
endif()

if(WIN32)

elseif(UNIX)
	set(HDF5_CMAKE_CURRENT_SOURCE_DIR /usr/local/ /usr) # there maybe more upto date library in these places
	include(GNUInstallDirs)
	find_package(PkgConfig REQUIRED QUIET)
endif()

find_package(ZLIB REQUIRED QUIET)


list(APPEND LIBS ${HDF5_C_LIBRARIES} ${ZLIB_LIBRARIES})
list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR} ${HDF5_INCLUDE_DIRS}  ${ZLIB_INCLUDE_DIRS})

include_directories(BEFORE ${CMAKE_SOURCE_DIR})



if( ${dlib_FOUND} )
	message( "-- Found: dlib library, added to examples" )
	add_executable(examples-dlib EXCLUDE_FROM_ALL "linalg/dlib.cpp")
	# dlib doesn't like c++17
	target_compile_options(examples-dlib PRIVATE "-w")
	target_link_libraries(examples-dlib ${LIBS} dlib::dlib)
	set_property(TARGET examples-dlib PROPERTY CXX_STANDARD 14)
	add_dependencies(examples examples-dlib)
endif()

if( ${Eigen3_FOUND} )
	message( "-- Found: eigen3 ${Eigen3_VERSION} added to examples" )
	add_executable(examples-eigen3 EXCLUDE_FROM_ALL "linalg/eigen3.cpp")
	include_directories(${EIGEN3_INCLUDE_DIR})
	target_link_libraries(examples-eigen3 ${LIBS} Eigen3::Eigen )
	add_dependencies(examples examples-eigen3)
endif()

if( ${ARMADILLO_FOUND} )
	message( "-- Found: armadillo ${ARMADILLO_VERSION_STRING} added to examples" )
	add_executable(examples-arma EXCLUDE_FROM_ALL "linalg/arma.cpp")
	target_link_libraries(examples-arma ${LIBS})
	add_dependencies(examples examples-arma)
endif()

)



include(FindMPI)
if( MPI_FOUND AND HDF5_IS_PARALLEL )
	find_program(SLURM_SBATCH_COMMAND sbatch DOC "Path to the SLURM sbatch executable")
	find_program(SLURM_SRUN_COMMAND srun DOC "Path to the SLURM srun executable")
	find_program(SLURM_SACCTMGR_COMMAND sacctmgr DOC "Path to the SLURM sacctmgr executable")

	message("-- MPI and PHDF5 found: Parallel H5BENCH is enabled")
	message("-- using ${MPI_CXX_COMPILER} ${MPIEXEC_EXECUTABLE}")
	if(SLURM_SRUN_COMMAND AND SLURM_SBATCH_COMMAND)
		set(SLURM_FOUND true)
	endif()
	if( SLURM_FOUND )
		message("-- SLURM found, scheduling with ${SLURM_SBATCH_COMMAND} ${SLURM_SRUN_COMMAND}")
	elseif()
		message("-- SLURM NOT FOUND schedule the test with mpiexec -n proc_num ...")
	endif()

	## mpi throughput
	#add_executable(examples-mpi-throughput EXCLUDE_FROM_ALL "mpi/throughput.cpp")
	#target_link_libraries(examples-mpi-throughput ${LIBS} ${MPI_CXX_LIBRARIES} )
	#target_compile_definitions(examples-mpi-throughput PUBLIC ${MPI_CXX_COMPILE_DEFINITIONS})
	#target_include_directories(examples-mpi-throughput PUBLIC ${MPI_CXX_INCLUDE_DIRS})
	#add_dependencies(examples examples-mpi-throughput)
endif()

## capi ###
add_executable(examples-capi EXCLUDE_FROM_ALL "capi/capi.cpp")
target_link_libraries(examples-capi ${LIBS} )
add_dependencies(examples examples-capi)

## h5append ###
add_executable(examples-append EXCLUDE_FROM_ALL "append/append.cpp")
target_link_libraries(examples-append ${LIBS} )
add_dependencies(examples examples-append)

