file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ FILES ${HEADERS} ${SOURCES} )

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yustdafx.h /FIstdafx.h")
endif()

add_library(ECS STATIC ${SOURCES} ${HEADERS})
target_link_libraries(ECS AssetManagement Utilities)

target_include_directories(ECS PUBLIC . )

set_source_files_properties( stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h" )
add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)