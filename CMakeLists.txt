cmake_minimum_required(VERSION 3.5)

project(icl LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set( CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUSE_UNIX_OS)

add_library(icl STATIC zip/Zip.cpp util/Util.cpp)
target_include_directories (icl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/zip ${CMAKE_CURRENT_SOURCE_DIR}/util)
