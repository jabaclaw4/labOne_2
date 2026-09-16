cmake_minimum_required(VERSION 3.16)
project(labOne_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

if(ENABLE_SANITIZERS)
    message(STATUS "Sanitizers ENABLED")
    add_compile_options(-fsanitize=address,undefined -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
else()
    message(STATUS "Sanitizers DISABLED")
endif()

add_executable(labOne_2
        src/main.cpp
        test/test_unqptr.cpp
        test/test_shrdptr.cpp
        test/test_dynamicarray.cpp
        test/test_subtyping.cpp
        test/load_test.cpp
)