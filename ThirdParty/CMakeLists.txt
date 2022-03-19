#============
#ASSIMP
#============

include_directories(assimp/code/BoostWorkaround)
add_definitions(-DASSIMP_BUILD_BOOST_WORKAROUND)

set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

set(ASSIMP_OPT_BUILD_PACKAGES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(assimp)
set_property(TARGET assimp PROPERTY CXX_STANDARD 14)

set_target_properties(
	assimp
	UpdateAssimpLibsDebugSymbolsAndDLLs
	zlibstatic
	PROPERTIES FOLDER "external/assimp"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )



#============
#IMGUI
#============

file(GLOB IMGUI_SOURCES "imgui/*.cpp" "imgui/examples/imgui_impl_win32.cpp" )
file(GLOB IMGUI_HEADERS "imgui/imgui.h" "imgui/imgui_internal.h" "imgui/imstb_rectpack.h" "imgui/imstb_textedit.h" "imgui/imstb_truetype.h" "imgui/examples/imgui_impl_win32.h" )

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}/
	FILES 
	${IMGUI_HEADERS}
	${IMGUI_SOURCES})
	
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS} )
target_include_directories(imgui PUBLIC imgui/. )
target_include_directories(imgui PUBLIC . )

set_target_properties(
	imgui
	PROPERTIES FOLDER "external"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )



#============
#STB
#============

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb/)


#============
#TERATHON-MATH
#============

file(GLOB TERATHON_MATH_SOURCES "terathon-math/*.cpp" )
file(GLOB TERATHON_MATH_HEADERS "terathon-math/*.h" )

source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}/
	FILES 
	${TERATHON_MATH_HEADERS}
	${TERATHON_MATH_SOURCES})
	
add_library(terathon-math STATIC ${TERATHON_MATH_SOURCES} ${TERATHON_MATH_HEADERS} )
target_include_directories(terathon-math PUBLIC terathon-math/. )
target_include_directories(terathon-math PUBLIC . )

set_target_properties(
	terathon-math
	PROPERTIES FOLDER "external"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )



#============
#OTHERS
#============

set_target_properties(
	uninstall
	PROPERTIES FOLDER "CMakePredefinedTargets")