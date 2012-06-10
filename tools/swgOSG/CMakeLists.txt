file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

add_executable(swgOSG
    ${SOURCES}
)

target_link_libraries(swgOSG
    treLib
    meshLib
    ${OPENSCENEGRAPH_LIBRARIES}
    ${ZLIB_LIBRARY_RELEASE}
)

if(MSVC)
    configure_file(${PROJECT_SOURCE_DIR}/build-aux/user_project.vcxproj.in
        ${CMAKE_CURRENT_BINARY_DIR}/swgOSG.vcxproj.user @ONLY)
endif()
