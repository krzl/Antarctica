cmake_minimum_required(VERSION 3.13)
project(Antarctica)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CONFIGURATION_TYPES Debug;Release CACHE STRING "" FORCE)

if( NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
	message(SEND_ERROR "Unhandled architecture, use cmake with flag -A x64" )
endif()

set(dir ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

add_subdirectory(Antarctica)
add_subdirectory(Game)
add_subdirectory(ECS)

add_subdirectory(Animation)
add_subdirectory(AssetManagement)
add_subdirectory(Navigation)
add_subdirectory(Platform)
add_subdirectory(Physics)
add_subdirectory(Rendering)

add_subdirectory(Utilities)
add_subdirectory(ThirdParty)

target_link_libraries(Antarctica assimp)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)