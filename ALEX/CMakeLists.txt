cmake_minimum_required(VERSION 3.12)
project(alex)

set(CMAKE_CXX_STANDARD 14)

if(MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /arch:AVX2 /W1 /EHsc")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "-O3 -xHost")
else()
    # clang and gcc
    set(CMAKE_CXX_FLAGS "-O3 -march=native -Wall -Wextra")
endif()

include_directories(src/core)

#add_executable(example src/examples/main.cpp)
#add_executable(benchmark src/benchmark/main.cpp)
add_executable(runALEX Alex.cpp)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

#add_executable(test_alex
#        test/unittest_main.cpp)
#target_link_libraries(test_alex gtest_main)

# ***** VTune ********
# If you are using Vtune, make sure the path is correct
IF(VTUNE)
    include_directories(/opt/intel/inteloneapi/vtune/latest/sdk/include/)
    target_link_libraries(runALEX /opt/intel/inteloneapi/vtune/latest/sdk/lib64/libittnotify.a)
    target_link_libraries(runALEX dl) 
    ADD_DEFINITIONS(-DVTUNE)
ENDIF(VTUNE)

IF(TIME)
    ADD_DEFINITIONS(-DTIME)
ENDIF(TIME)

IF(CONSECUTIVE)
    ADD_DEFINITIONS(-DCONSECUTIVE)
ENDIF(CONSECUTIVE)
