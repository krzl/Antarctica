file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ FILES ${HEADERS} ${SOURCES} )

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yustdafx.h /FIstdafx.h")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
endif()

add_executable(Editor WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(Editor Antarctica)

target_include_directories(Editor PUBLIC . )

set_source_files_properties( stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h" )
add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)