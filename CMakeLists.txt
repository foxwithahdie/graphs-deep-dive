cmake_minimum_required(VERSION 3.19)

project(graphs-deep-dive VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)

# address + undefined behavior sanitizers; catches double frees, leaks and
# out of bounds reads at runtime
add_compile_options(-fsanitize=address,undefined -g)
add_link_options(-fsanitize=address,undefined)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)