cmake_minimum_required(VERSION 3.14)

project(ePugStation VERSION 0.1.0 LANGUAGES C CXX)

include_directories(src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)

#target_include_directories(ePugStation PUBLIC src)
#target_include_directories(Architecture PUBLIC src)
