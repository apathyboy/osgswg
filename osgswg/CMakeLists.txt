file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

include_directories(
    ${Boost_INCLUDE_DIR}
)

add_library(osgswg STATIC ${SOURCES})

target_link_libraries(osgswg
    treLib
    meshLib
    ${Boost_LIBRARIES}
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ZLIB_LIBRARY_RELEASE}
)
