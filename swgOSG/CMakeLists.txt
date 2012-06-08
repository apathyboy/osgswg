file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

add_executable(swgOSG
    ${SOURCES}
)

include_directories(
    ${OSG_INCLUDE_DIR}
)

target_link_libraries(swgOSG
    treLib
    meshLib
    ${OSG_LIBRARY}
)
