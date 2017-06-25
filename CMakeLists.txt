
# Requires "FindSDL2.cmake" and "FindSDL2_image.cmake" in ./cmake/
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "./cmake")

cmake_minimum_required(VERSION 3.6)
project(Adventure)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/main.cpp src/area.cpp src/areas.cpp src/player.cpp src/slime.cpp src/arrow.cpp src/boomerang.cpp)

set(AREA_FILES map/InsideHome.area map/OutsideHome.area map/T1Entrance.area map/T1SlimeRoom.area)

add_executable( Adventure ${SOURCE_FILES} )

add_custom_command(
    OUTPUT src/areas.hpp src/areas.cpp
    COMMAND ./genAreas.py
    DEPENDS ${AREA_FILES}
)


find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(Adventure ${SDL2_LIBRARY}
                                ${SDL2_IMAGE_LIBRARIES})