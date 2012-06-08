cmake_minimum_required(VERSION 2.8.6)

project(swgOSG CXX)

# set the path for custom cmake scripts
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build-aux/cmake")

# include common scripts
include(GroupSourcesByFilesystem)
include(CheckIncludeFiles)

find_package(OSG REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/swgOSG
)

add_subdirectory(meshlib)
add_subdirectory(trelib)
add_subdirectory(swgOSG)