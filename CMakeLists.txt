cmake_minimum_required(VERSION 3.14)

project(ePugStation VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(_src_root_path "${CMAKE_CURRENT_SOURCE_DIR}/src")

## conan
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

include_directories(${_src_root_path})

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${_src_root_path}/*.c*"
    "${_src_root_path}/*.h*"
)

add_executable(ePugStation ${_source_list})
target_link_libraries(ePugStation CONAN_PKG::sdl2)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()
