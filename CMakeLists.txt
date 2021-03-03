cmake_minimum_required(VERSION 3.19.3)
project(MyHashTable)

set(CMAKE_BUILD_TYPE, "Debug")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRES ON)

add_subdirectory(chaining)
add_subdirectory(StringHash)
