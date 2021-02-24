cmake_minimum_required(VERSION 3.5)

project(icl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
    add_definitions(-DUSE_UNIX_OS)
endif()

if(WIN32)
    add_definitions(-DUSE_WINDOWS_OS)
endif()

add_library(
    icl
    STATIC 
    zip/Zip.cpp
    util/Util.cpp 
    security/Base64Util.cpp
    jsengine/JSEngine.cpp
    jsengine/duktape.c
    jsengine/duk_module_duktape.c
)

add_definitions(-fPIC)

target_include_directories (icl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/zip ${CMAKE_CURRENT_SOURCE_DIR}/util ${CMAKE_CURRENT_SOURCE_DIR}/jsengine ${CMAKE_CURRENT_SOURCE_DIR}/security)
