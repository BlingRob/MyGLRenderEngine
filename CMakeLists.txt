cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
project(GraphicsEngine)
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})	

#Build my files
file(GLOB_RECURSE SOURCE_FILES
			${CMAKE_SOURCE_DIR}/src/*.c
			${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE HEADER_FILES
		${CMAKE_SOURCE_DIR}/src/*.h
		${CMAKE_SOURCE_DIR}/src/*.hpp)


#OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS});
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OpenGL_FOUND)
	message(Error "OpenGL not found")
endif(NOT OpenGL_FOUND)	
#GLM
set(GLM_DIR ${CMAKE_SOURCE_DIR}/Deps/Include/GLM/)
find_package(GLM REQUIRED)
#ASSIMP

#Build dependencies			
link_directories(${CMAKE_SOURCE_DIR}/Deps/Lib/)	
include_directories(${CMAKE_SOURCE_DIR}/Deps/Include/)



find_package(ASSIMP REQUIRED)
#STB,STBI, galogen
add_library(STB_IMAGE ${CMAKE_SOURCE_DIR}/Deps/Include/STB/stb_image.cpp)
add_library(STBI_DDS ${CMAKE_SOURCE_DIR}/Deps/Include/STB/stbi_DDS_aug_c)


set(LIBS OpenGL assimp STB_IMAGE STBI_DDS GLM)
target_link_libraries(${PROJECT_NAME} ${LIBS})


		