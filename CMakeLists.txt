cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(bt2 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CONFIGURATION_TYPES Debug Release)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt6 COMPONENTS Core Network Quick QuickControls2 Widgets Qml Svg REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/")
include(GetGitRevisionDescription)

find_package(box2d REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/deps/")

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
