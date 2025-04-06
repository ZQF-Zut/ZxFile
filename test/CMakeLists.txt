find_package(Catch2 CONFIG)
if(NOT Catch2_FOUND)
    message(STATUS "⚠️  Catch2 not found!")
    message(STATUS "➡️  skip unit tests")
    return()
endif()

# Project
project(Zut_ZxFile_Test)

# Executable
add_executable("${PROJECT_NAME}" "test.cpp")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>:/utf-8>)

# Warning
target_compile_options(${PROJECT_NAME} PRIVATE $<$<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>:/W4>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra>)

# External Libraries
target_link_libraries("${PROJECT_NAME}" PRIVATE Catch2::Catch2 Catch2::Catch2WithMain Zut::ZxFile)
include(Catch)
catch_discover_tests("${PROJECT_NAME}")