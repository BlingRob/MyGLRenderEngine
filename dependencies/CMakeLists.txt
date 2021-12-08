cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs CACHE PATH " " FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs CACHE PATH " " FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exe CACHE PATH " " FORCE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

##########################Assimp build#####################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/assimp ${CMAKE_CURRENT_BINARY_DIR}/assimp)

set_target_properties(assimp
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/exe"
    ASSIMP_BUILD_TESTS OFF
    ASSIMP_NO_EXPORT ON
    ASSIMP_BUILD_ASSIMP_TOOLS OFF
    ASSIMP_INSTALL OFF
    ASSIMP_SHARED_LIBS ON
)	

###########################################################
###########################OpenGL##########################
add_library(OpenGL STATIC ${CMAKE_CURRENT_SOURCE_DIR}/gl/gl.h ${CMAKE_CURRENT_SOURCE_DIR}/gl/gl.c)
###########################################################
######################Build imgui##########################
#copy configuration
file(COPY ${CMAKE_SOURCE_DIR}/config/imconfig.h
     		  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.h
		  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl.h
		  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
		  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_sdl.cpp
		  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
#collect imgui's source files 
file(GLOB IMGUI_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h)
file(GLOB IMGUI_SOURCE  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp)
#create library
add_library(IMGUI STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCE})
###########################################################
############################STB############################
add_library(STB STATIC ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image.h ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image.cpp)
###########################################################
############################SDL############################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sqlite ${CMAKE_CURRENT_BINARY_DIR}/sqlite)
####################SQLite#################################
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sdl ${CMAKE_CURRENT_BINARY_DIR}/sdl)
############################SDL############################
# Put all libraries into a variable
set(LIBS IMGUI assimp SDL2 SDL2main OpenGL SQLLite STB PARENT_SCOPE)

# Include target directories
target_include_directories(IMGUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sdl/include/)
