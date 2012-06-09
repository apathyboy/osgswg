cmake_minimum_required(VERSION 2.8.6)

project(swgOSG CXX)

# set the path for custom cmake scripts
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build-aux/cmake")

# include common scripts
include(GroupSourcesByFilesystem)
include(CheckCXXCompilerFlag)
include(CheckIncludeFiles)

option(TREAT_WARNINGS_AS_ERRORS
    "Treat all warnings as errors" ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    check_cxx_compiler_flag(-std=c++0x HAX_CPP0X_SUPPORT)
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

    if(TREAT_WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic-errors")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic-errors")
        message(STATUS "Treating all warnings as errors")
    endif()
endif()

if(MSVC)
    set(_WIN32_WINNT 0x0601 CACHE INTERNAL "Setting _WIN32_WINNT to 0x0601 for Windows Vista minimum APIs")
    add_definitions(/D _WIN32_WINNT=${_WIN32_WINNT})
    message(STATUS "Set minimum windows version to Vista")

    if(TREAT_WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
        message(STATUS "Treating all warnings as errors")
    endif()
endif()

if(DEFINED OSGDIR)
    set(ENV{OSGDIR} ${OSGDIR})
endif()

find_package(OpenSceneGraph 3.0.0 REQUIRED osgAnimation osgViewer osgText osgDB osgGA)
find_package(ZLib REQUIRED)

include_directories(
   ${PROJECT_SOURCE_DIR}/meshlib ${PROJECT_SOURCE_DIR}/trelib ${PROJECT_SOURCE_DIR}/swgOSG
)

if(MSVC)
	get_filename_component(ZLIB_DLL_PATH ${ZLIB_LIBRARY_RELEASE} PATH)
	get_filename_component(OSG_DLL_PATH ${OSG_LIBRARY}/../bin PATH)
endif()

add_subdirectory(meshlib)
add_subdirectory(trelib)
add_subdirectory(swgOSG)
