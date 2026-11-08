cmake_minimum_required(VERSION 3.16)
project(MarioSokoban LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL REQUIRED sdl)
pkg_check_modules(SDL_IMAGE REQUIRED SDL_image)
pkg_check_modules(SDL_TTF REQUIRED SDL_ttf)

add_executable(mario-sokoban main.cpp)
set_target_properties(mario-sokoban PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/package")
target_include_directories(mario-sokoban PRIVATE ${SDL_INCLUDE_DIRS} ${SDL_IMAGE_INCLUDE_DIRS} ${SDL_TTF_INCLUDE_DIRS})
target_compile_options(mario-sokoban PRIVATE ${SDL_CFLAGS_OTHER} ${SDL_IMAGE_CFLAGS_OTHER} ${SDL_TTF_CFLAGS_OTHER})
target_link_directories(mario-sokoban PRIVATE ${SDL_LIBRARY_DIRS} ${SDL_IMAGE_LIBRARY_DIRS} ${SDL_TTF_LIBRARY_DIRS})
target_link_libraries(mario-sokoban PRIVATE ${SDL_LIBRARIES} ${SDL_IMAGE_LIBRARIES} ${SDL_TTF_LIBRARIES})
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
)
