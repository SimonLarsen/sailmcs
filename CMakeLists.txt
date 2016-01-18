cmake_minimum_required(VERSION 3.1)
project(sailmcs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type specified. Defaulting to Release.")
	set(CMAKE_BUILD_TYPE "Release")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fopenmp")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse3")
endif()

find_package(Boost REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/tclap/include
	${CMAKE_SOURCE_DIR}/graphio/include
	${Boost_INCLUDE_DIRS}
)

add_executable(sailmcs
	src/SailMCS.cpp
)

target_link_libraries(sailmcs
	sailmcs_ils
	sailmcs_ls
)

add_subdirectory(src/sailmcs)
add_subdirectory(man)
