cmake_minimum_required(VERSION 3.21)
project(MarioSokobanSDL3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

add_executable(mario-sokoban main.cpp)
set_target_properties(mario-sokoban PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/package"
)
target_link_libraries(mario-sokoban PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

set(PACKAGE_DIR "$<TARGET_FILE_DIR:mario-sokoban>")
if(WIN32)
    add_custom_command(TARGET mario-sokoban POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:mario-sokoban> "${PACKAGE_DIR}"
        COMMAND_EXPAND_LISTS
    )
endif()
add_custom_command(TARGET mario-sokoban POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/sprites_mario_sokoban" "${PACKAGE_DIR}/sprites_mario_sokoban"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/niveaux perso" "${PACKAGE_DIR}/niveaux perso"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/NotoSans.ttf" "${PACKAGE_DIR}/NotoSans.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/NotoSans-OFL.txt" "${PACKAGE_DIR}/NotoSans-OFL.txt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSES.md" "${PACKAGE_DIR}/LICENSES.md"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/SDL3-zlib.txt" "${PACKAGE_DIR}/SDL3-zlib.txt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/SDL3-ttf-zlib.txt" "${PACKAGE_DIR}/SDL3-ttf-zlib.txt"
)
