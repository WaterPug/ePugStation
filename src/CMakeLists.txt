include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h*"
)

set(SHADER_FILES ${CMAKE_SOURCE_DIR}/shaders/vertex.glsl ${CMAKE_SOURCE_DIR}/shaders/fragment.glsl)

find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(ePugStation ${_source_list} ${SHADER_FILES})
target_link_libraries(ePugStation PRIVATE glad::glad)
target_link_libraries(ePugStation PRIVATE SDL2::SDL2 SDL2::SDL2main)

source_group("Shaders" FILES ${SHADER_FILES})

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
    source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()