cmake_minimum_required(VERSION 3.12)
project(art)

set(CMAKE_CXX_STANDARD 14)

if(MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /arch:AVX2 /W1 /EHsc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "-O3 -xHost")
else()
    # clang and gcc
    set(CMAKE_CXX_FLAGS "-O3 -march=native -Wall -Wextra")
endif()

include_directories(src/)

#add_executable(example src/examples/main.cpp)
#add_executable(benchmark src/benchmark/main.cpp)

add_executable(runART art.cpp)

# ***** VTune ********
# If you are using Vtune, make sure the path is correct
IF(VTUNE)
    include_directories(/opt/intel/inteloneapi/vtune/latest/sdk/include/)
    target_link_libraries(runART /opt/intel/inteloneapi/vtune/latest/sdk/lib64/libittnotify.a)
    target_link_libraries(runART dl) 
    ADD_DEFINITIONS(-DVTUNE)
ENDIF(VTUNE)

IF(TIME)
    ADD_DEFINITIONS(-DTIME)
ENDIF(TIME)

IF(CONSECUTIVE)
    ADD_DEFINITIONS(-DCONSECUTIVE)
ENDIF(CONSECUTIVE)
