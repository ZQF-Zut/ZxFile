# Project
project(zqf_zut_zxfile_tryout VERSION 1.0 LANGUAGES C CXX)

# Executable
add_executable(${PROJECT_NAME} "main.cpp")

# Compiler
include("zqf_settings")
zqf_set_encoding(${PROJECT_NAME} PRIVATE UTF8)
zqf_set_warning(${PROJECT_NAME} PRIVATE AllExtra)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# External Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ZQF::Zut::ZxFile)
