file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

add_library(osgswg MODULE ${SOURCES})

target_link_libraries(osgswg
    treLib
    meshLib
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ZLIB_LIBRARY_RELEASE}
)

