cmake_minimum_required(VERSION 3.16)

project(naaf_approx_seal)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SEAL 3.6 REQUIRED) # Using FHE-MP-CNN implementation

# Include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
set(SOURCES ${SOURCES} main.cpp)

# Add executables
add_executable(runNaafs ${SOURCES})

# Compile option
target_compile_options(runNaafs PRIVATE -Wall -O3 -pthread -march=native)
# target_compile_options(runNaafs PRIVATE -Wall -O3 -Wextra -Wunused) # for debugging

# Link related libraries
target_link_libraries(runNaafs PRIVATE ntl gmp m pthread SEAL::seal)
target_include_directories(runNaafs PUBLIC /usr/local/include /usr/local/include/NTL ${COMMON_HEADER_DIR} ${COMP_HEADER_DIR})
