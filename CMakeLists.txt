cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
project(GraphicsEngine)	

###########################OpenGL##########################
if(LINUX)
	find_package(OpenGL REQUIRED)
	include_directories(${OpenGL_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS})
	if(NOT OpenGL_FOUND)
		message(Error "OpenGL not found")
	endif(NOT OpenGL_FOUND)	
elseif(WIN32)
	add_library(OpenGL STATIC ${CMAKE_SOURCE_DIR}/deps/gl/gl.h ${CMAKE_SOURCE_DIR}/deps/gl/gl.c)
endif()
###########################################################
###################Build my files##########################
file(GLOB_RECURSE SOURCE_FILES
			src/*.c
			src/*.cpp)

file(GLOB_RECURSE HEADER_FILES
		src/*.h
		src/*.hpp)
###########################################################
###############Including main directories##################
include_directories(${CMAKE_SOURCE_DIR}/subprojects/)
include_directories(${CMAKE_SOURCE_DIR}/deps/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps/libs CACHE PATH " " FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps/libs CACHE PATH " " FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/exe CACHE PATH " " FORCE)
######################Build imgui##########################
include_directories(${CMAKE_SOURCE_DIR}/cmake/imgui/)
#copy my configuration
file(COPY ${CMAKE_SOURCE_DIR}/config/imconfig.h
     DESTINATION ${CMAKE_SOURCE_DIR}/cmake/imgui)
file(COPY ${CMAKE_SOURCE_DIR}/cmake/imgui/backends/imgui_impl_opengl3.h
		  DESTINATION ${CMAKE_SOURCE_DIR}/cmake/imgui/)
file(COPY ${CMAKE_SOURCE_DIR}/cmake/imgui/backends/imgui_impl_sdl.h
		  DESTINATION ${CMAKE_SOURCE_DIR}/cmake/imgui/)
file(COPY ${CMAKE_SOURCE_DIR}/cmake/imgui/backends/imgui_impl_opengl3.cpp
		  DESTINATION ${CMAKE_SOURCE_DIR}/cmake/imgui/)
file(COPY ${CMAKE_SOURCE_DIR}/cmake/imgui/backends/imgui_impl_sdl.cpp
		  DESTINATION ${CMAKE_SOURCE_DIR}/cmake/imgui/)
#collect imgui's source files 
file(GLOB IMGUI_HEADERS ${CMAKE_SOURCE_DIR}/cmake/imgui/*.h)
file(GLOB IMGUI_SOURCE ${CMAKE_SOURCE_DIR}/cmake/imgui/*.cpp)

#create library
add_library(IMGUI STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCE})
set_target_properties(IMGUI PROPERTIES LINKER_LANGUAGE Cxx) 
###########################################################
##########################Assimp build#####################
include_directories(${CMAKE_SOURCE_DIR}/cmake/assimp/include/)
include_directories(${CMAKE_SOURCE_DIR}/subprojects/assimp/include/)
add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/assimp ${CMAKE_SOURCE_DIR}/subprojects/assimp EXCLUDE_FROM_ALL)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_SHARED_LIBS ON)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_NO_EXPORT ON)
set_target_properties(assimp
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/deps/libs"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/deps/libs"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/exe"
)
###########################################################
##########################Glm##############################
include_directories(${CMAKE_SOURCE_DIR}/cmake/glm/)
###########################################################
############################STB############################
add_library(STB STATIC ${CMAKE_SOURCE_DIR}/deps/stb/stb_image.h ${CMAKE_SOURCE_DIR}/deps/stb/stb_image.cpp)
###########################################################
############################SDL############################
add_subdirectory(${CMAKE_SOURCE_DIR}/cmake/sdl ${CMAKE_SOURCE_DIR}/subprojects/sdl)
###########################################################

# Put all libraries into a variable
set(LIBS IMGUI assimp SDL2 SDL2main OpenGL STB)

if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${HEADER_FILES})
else()
	add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} )
endif()
# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})
# Include target directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/cmake/sdl/include/)
target_include_directories(IMGUI PUBLIC ${CMAKE_SOURCE_DIR}/cmake/sdl/include/)