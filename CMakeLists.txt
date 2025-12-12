# cmake_minimum_required(VERSION 3.10)
# project(ride_matching_engine)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native -pipe")
# include_directories(${PROJECT_SOURCE_DIR}/include)
# file(GLOB SRC_FILES src/*.cpp)
# add_executable(ride_matching_simulator ${SRC_FILES})

cmake_minimum_required(VERSION 3.10)
project(ride_matching_engine)

set(CMAKE_CXX_STANDARD 17)

# 1. REMOVED the GCC-specific flags (-march=native, -pipe) 
#    Visual Studio handles optimization automatically in Release mode.

# 2. Point to the include folder
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 3. Explicitly list BOTH files to fix the Linker (LNK2019) error
#    This ensures VS knows exactly what to compile.
add_executable(ride_matching_simulator 
    src/main.cpp 
    src/MatchingEngine.cpp
)