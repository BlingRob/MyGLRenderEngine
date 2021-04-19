#https://github.com/jineyne/glad-cmake/blob/master/CMakeLists.txt
project(galogen)

set(GALOGEN_BUILD_SHARED OFF CACHE BOOL "build glad to shared")

if(GALOGEN_BUILD_SHARED)
    add_library(galogen SHARED
        "gl.h" 
        "gl.c")
    target_compile_definitions(galogen
    PUBLIC GALOGEN_GLAPI_EXPORT 
    PRIVATE GALOGEN_GLAPI_EXPORT_BUILD)
else()
    add_library(galogen STATIC
        "gl.h" 
        "gl.c")
endif()
target_include_directories(galogen PUBLIC "include")