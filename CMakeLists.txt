cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(PROJECT_NAME LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wpedantic -g")

find_package(SFML 2 REQUIRED network audio graphics window system)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
endif()

add_compile_definitions(MSGPACK_USE_BOOST)
include_directories("${PROJECT_SOURCE_DIR}/deps/msgpack-c/include/")

add_subdirectory(src)
