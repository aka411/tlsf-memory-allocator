#TLSF Allocator cmake file


cmake_minimum_required(VERSION  3.14)

project(tlsf-allocator)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

# Set a common output directory for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "Force Google Test to use shared (DLL) run-time lib.")
endif()

add_library(tlsf-allocator STATIC 

src/tlsf.cpp

)

add_library(tlsf::tlsf-allocator ALIAS tlsf-allocator)

target_include_directories(tlsf-allocator PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)



#test folder
enable_testing()
add_subdirectory(test)



#demo configuration

add_executable(demo-app 

 ${CMAKE_CURRENT_SOURCE_DIR}/demo/demo_app.cpp

)

target_link_libraries(demo-app PRIVATE tlsf::tlsf-allocator)

add_executable(benchmark-app

  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/benchmark.cpp
  )
  target_link_libraries(benchmark-app PRIVATE
  tlsf::tlsf-allocator
  )