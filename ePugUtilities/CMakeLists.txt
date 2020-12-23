add_library(ePugUtilities STATIC src/OpUtilities.cpp)

target_include_directories(ePugUtilities 
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)