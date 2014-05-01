cmake_minimum_required (VERSION 2.8)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project (clv)
set (Tutorial_VERSION_MAJOR 0)
set (Tutorial_VERSION_MINOR 0)

add_subdirectory(source)

include_directories("${PROJECT_BINARY_DIR}")

