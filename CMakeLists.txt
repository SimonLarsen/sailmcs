cmake_minimum_required(VERSION 2.8)
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

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fopenmp")
	list(APPEND STANDARD_LIBRARIES m)
endif()

find_package(Boost REQUIRED)
find_program(GZIP_BINARY NAMES gzip PATHS /bin /usr/bin /usr/local/bin)
find_program(HELP2MAN_BINARY NAMES help2man PATHS /bin /usr/bin /usr/local/bin)

include_directories(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/tclap/include
	${CMAKE_SOURCE_DIR}/graphio/include
	${Boost_INCLUDE_DIRS}
)

add_executable(sailmcs src/SailMCS.cpp)

target_link_libraries(sailmcs
	sailmcs_ils
	sailmcs_ls
	${STANDARD_LIBRARIES}
)

add_subdirectory(src/sailmcs)
add_subdirectory(man)

install(TARGETS sailmcs RUNTIME DESTINATION bin)
install(FILES ${CMAKE_BINARY_DIR}/man/sailmcs.7.gz DESTINATION share/man/man7 OPTIONAL)
