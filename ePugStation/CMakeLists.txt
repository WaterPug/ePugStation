add_executable(ePugStation 
                src/main.cpp
                src/Cop0.cpp
                src/CPU.cpp
                src/DMA.cpp
                src/Interconnect.cpp)

find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(SDL2 REQUIRED)

target_link_libraries(ePugStation PRIVATE ePugUtilities OpenGL::GL glad::glad SDL2::SDL2 SDL2::SDL2main)