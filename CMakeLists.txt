cmake_minimum_required(VERSION 3.21)
project(MarioSokobanSDL3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG QUIET)
find_package(SDL3_ttf CONFIG QUIET)
if((NOT SDL3_FOUND OR NOT SDL3_ttf_FOUND) AND WIN32 AND MINGW)
    # A clean CLion/MinGW checkout downloads the official prebuilt development
    # SDKs into the build folder. GitHub Actions uses setup-sdl's 3-latest.
    include(FetchContent)
    if(NOT SDL3_FOUND)
        FetchContent_Declare(SDL3WindowsSDK
            URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.18/SDL3-devel-3.4.18-mingw.tar.gz
            URL_HASH SHA256=049b9dd711e3fb3f8d84feb998dfec54a75e17cf539c4a80ff4d058d839771cf
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(SDL3WindowsSDK)
        find_package(SDL3 CONFIG REQUIRED PATHS "${sdl3windowssdk_SOURCE_DIR}/cmake" NO_DEFAULT_PATH)
    endif()
    if(NOT SDL3_ttf_FOUND)
        FetchContent_Declare(SDL3TtfWindowsSDK
            URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-devel-3.2.2-mingw.tar.gz
            URL_HASH SHA256=bb57f26787d6a2e108158562feb061fcdf6f68a110f9c8cf9af42ff343d4e41c
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(SDL3TtfWindowsSDK)
        find_package(SDL3_ttf CONFIG REQUIRED PATHS "${sdl3ttfwindowssdk_SOURCE_DIR}/cmake" NO_DEFAULT_PATH)
    endif()
endif()

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
