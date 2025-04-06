# ZxFile Main
cmake_minimum_required(VERSION 3.25)

# Project
project(Zut_ZxFile_Main)

# Config
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include("cmake/ZQFSettings.cmake")
endif()

# Source
add_subdirectory("src")

# Dev
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory("test")
    add_subdirectory("tryout")
endif()
