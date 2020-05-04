include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h*"
)

find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(ePugStation ${_source_list})
target_link_libraries(ePugStation PRIVATE glad::glad)
target_link_libraries(ePugStation PRIVATE SDL2::SDL2 SDL2::SDL2main)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()
