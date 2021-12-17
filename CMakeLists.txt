cmake_minimum_required(VERSION 3.17)
project(GraphicsEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
###################Collect project files##########################
file(GLOB_RECURSE SOURCE_FILES
			RenderEngine/*.c
			RenderEngine/*.cpp)
file(GLOB_RECURSE HEADER_FILES
			RenderEngine/*.h
			RenderEngine/*.hpp)
		
source_group(TREE ${CMAKE_SOURCE_DIR}/RenderEngine FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/RenderEngine FILES ${HEADER_FILES})
###########################################################
#Resolve all dependencies
add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/)
#include dependencies headers
include_directories(${CMAKE_SOURCE_DIR}/dependencies/)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/sqlite)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/assimp/include)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/glm/)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/imgui)
include_directories(${CMAKE_SOURCE_DIR}/dependencies/imgui-filebrowser)

find_package(OpenMP REQUIRED)

if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES})
else()
	add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
endif()

# Define the link libraries
#LIBS from subdirs
target_link_libraries(${PROJECT_NAME} ${LIBS} "${OpenMP_CXX_FLAGS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/sdl/include/)
###Collect resources
file(GLOB RESOURCE_FILES ${CMAKE_SOURCE_DIR}/Shaders/*)
set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE "${RESOURCE_FILES}")
target_compile_options(${PROJECT_NAME} PRIVATE "${OpenMP_CXX_FLAGS}")
###########################################################
############################Special unix###################
if(NOT WIN32)
	list(APPEND CMAKE_CXX_FLAGS "-pthread -lGL -std=c++17 ${CMAKE_CXX_FLAGS}")
	#set(CMAKE_Cxx_CREATE_STATIC_LIBRARY OFF)
endif()
