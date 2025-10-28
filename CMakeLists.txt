#TLSF Allocator cmake file


cmake_minimum_required(VERSION  3.14)

project(tlsf-allocator)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Set a common output directory for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


add_library(tlsf-allocator STATIC 

src/tlsf.cpp
)


add_library(tlsf::tlsf-allocator ALIAS tlsf-allocator)

target_include_directories(tlsf-allocator PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)





option(TLSF_BUILD_TESTS "Build the unit tests for the TLSF Allocator." ON)


# Option to enable building the benchmark application
option(TLSF_BUILD_BENCHMARK "Build the benchmark application for the TLSF Allocator." ON)






if(TLSF_BUILD_TESTS)
  
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "Force Google Test to use shared (DLL) run-time lib.")
 endif()

enable_testing()
   #test folder
add_subdirectory(test)
endif()






if(TLSF_BUILD_BENCHMARK)

  FetchContent_Declare(
    google-benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.9.4  #  specific version for stability
   )

  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable testing of the benchmark library." FORCE)
  set(BENCHMARK_USE_LIBCXX OFF CACHE BOOL "Disable Build and test using libc++ as the standard library." FORCE)
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable building the unit tests which depend on gtest" FORCE)
  set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "Disable bundled GoogleTest. If disabled, the find_package(GTest) will be used." FORCE)

  FetchContent_MakeAvailable(google-benchmark)





  add_executable(benchmark-app

    ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/benchmark.cpp
  )


  target_link_libraries(benchmark-app PRIVATE
  tlsf::tlsf-allocator
  benchmark::benchmark
  )

endif()
