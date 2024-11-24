cmake_minimum_required(VERSION 3.2...3.28)

project(AhoCorasick)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options("-Wall;-Wextra;-pedantic;-Werror")

# Sanitizers
add_compile_options("-fno-omit-frame-pointer;-fsanitize=address;-fsanitize=undefined")
add_link_options("-fsanitize=address;-fsanitize=undefined")

add_library(aho_corasick STATIC
    src/aho_corasick.cpp)
target_include_directories(aho_corasick PUBLIC src/)

add_executable(aho_corasick_test
    test.cpp)
target_link_libraries(aho_corasick_test PRIVATE aho_corasick)
