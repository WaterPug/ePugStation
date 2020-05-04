cmake_minimum_required(VERSION 3.2)

project(ePugStation CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
# Compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Static analyzers options
include(cmake/StaticAnalyzers.cmake)

#include(cmake/Conan.cmake)
#run_conan()

set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories(${_src_root_path})

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
)

find_package(glad CONFIG REQUIRED)

add_executable(ePugStation ${_source_list})
target_link_libraries(ePugStation PRIVATE glad::glad)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()
