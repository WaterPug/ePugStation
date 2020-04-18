cmake_minimum_required(VERSION 3.14)

project(ePugStation VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Taken from : https://stackoverflow.com/questions/32645217/visual-studio-as-an-editor-for-cmake-friendly-project
set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/src")

include_directories(${_src_root_path})

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
)

add_library(ePugStation ${_source_list})

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()
