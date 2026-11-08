cmake_minimum_required(VERSION 3.20)
project(MarioSokobanSDL3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

add_executable(mario-sokoban main.cpp)
set_target_properties(mario-sokoban PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/package")
target_link_libraries(mario-sokoban PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(mario-sokoban PRIVATE -finput-charset=ISO-8859-1)
endif()

set(PACKAGE_DIR "$<TARGET_FILE_DIR:mario-sokoban>")
add_custom_command(TARGET mario-sokoban POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/sprites_mario_sokoban" "${PACKAGE_DIR}/sprites_mario_sokoban"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/niveaux perso" "${PACKAGE_DIR}/niveaux perso"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data" "${PACKAGE_DIR}/data"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/caisse.bmp" "${PACKAGE_DIR}/caisse.bmp"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/NotoSans.ttf" "${PACKAGE_DIR}/NotoSans.ttf"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/règles.txt" "${PACKAGE_DIR}/règles.txt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/règles.docx" "${PACKAGE_DIR}/règles.docx"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSES.md" "${PACKAGE_DIR}/LICENSES.md"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/NotoSans-OFL.txt" "${PACKAGE_DIR}/NotoSans-OFL.txt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/SDL-1.2-LGPL-2.1.txt" "${PACKAGE_DIR}/SDL-1.2-LGPL-2.1.txt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/SDL-image-zlib.txt" "${PACKAGE_DIR}/SDL-image-zlib.txt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/SDL-ttf-zlib.txt" "${PACKAGE_DIR}/SDL-ttf-zlib.txt"
)
