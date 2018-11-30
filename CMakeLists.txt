
cmake_minimum_required(VERSION 3.10)

project(swgOSG VERSION 1.0.0)

find_package(OpenSceneGraph 3.0.0 COMPONENTS osgAnimation osgViewer osgText osgDB osgGA REQUIRED)

add_subdirectory(meshlib)
add_subdirectory(trelib)

add_executable(swgOSG
    swgOSG/swgOSG.cpp
    swgOSG/swgRepository.cpp
)

target_include_directories(swgOSG PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/swgOSG ${OSG_INCLUDE_DIR})

target_link_libraries(swgOSG PRIVATE  meshLib::meshLib treLib::treLib ${OPENSCENEGRAPH_LIBRARIES})
