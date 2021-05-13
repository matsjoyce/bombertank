cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(PROJECT_NAME LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -g")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Quick QuickControls2 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Quick QuickControls2 REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_definitions(MSGPACK_USE_BOOST)

find_package(docopt COMPONENTS CXX REQUIRED)
find_package(box2d REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/deps/" "${PROJECT_SOURCE_DIR}/deps/msgpack-c/include/")

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

message(STATUS "${DOCOPT_INCLUDE_DIRS}=${BOX2D_INCLUDE_DIRS}")
