cmake_minimum_required(VERSION 3.16)
project(labOne_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Заголовки лежат в корне (или поменяйте на include_directories(include), если вынесете в папку include)
include_directories(${CMAKE_SOURCE_DIR})

# Основная программа (если она вам вообще нужна отдельно от тестов)
add_executable(labOne_2 src/main.cpp)

# Тесты — отдельный исполняемый файл
add_executable(test
        test/test_unqptr.cpp
        test/test_shrdptr.cpp
        test/test_dynamicarray.cpp
        test/test_subtyping.cpp
)