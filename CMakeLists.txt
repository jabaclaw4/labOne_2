cmake_minimum_required(VERSION 3.16)
project(labOne_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})


add_executable(labOne_2
        src/main.cpp
        test/test_unqptr.cpp
        test/test_shrdptr.cpp
        test/test_dynamicarray.cpp
        test/test_subtyping.cpp
        test/load_test.cpp
)