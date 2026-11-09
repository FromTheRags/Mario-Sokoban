cmake_minimum_required(VERSION 3.21)
project(MarioSokobanSDL3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND AND WIN32)
    # A clean Windows checkout (CLion with its bundled MinGW *or* with the
    # Visual Studio toolchain) downloads the official prebuilt development SDK
    # into the build folder. GitHub Actions installs SDL with setup-sdl.
    if(MSVC)
        set(SDL3_SDK_URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.18/SDL3-devel-3.4.18-VC.zip)
        set(SDL3_SDK_HASH 78d84602ae616cfe26b33a73b7d3b9b6b56e8a6650e53d92c9c6ca938f747acd)
    elseif(MINGW)
        set(SDL3_SDK_URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.18/SDL3-devel-3.4.18-mingw.tar.gz)
        set(SDL3_SDK_HASH 049b9dd711e3fb3f8d84feb998dfec54a75e17cf539c4a80ff4d058d839771cf)
    else()
        message(FATAL_ERROR "SDL 3 was not found. Use the MinGW or Visual Studio toolchain, "
                            "or set CMAKE_PREFIX_PATH to an SDL 3 installation.")
    endif()
    include(FetchContent)
    FetchContent_Declare(SDL3WindowsSDK
        URL ${SDL3_SDK_URL}
        URL_HASH SHA256=${SDL3_SDK_HASH}
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(SDL3WindowsSDK)
    find_package(SDL3 CONFIG REQUIRED PATHS "${sdl3windowssdk_SOURCE_DIR}/cmake" NO_DEFAULT_PATH)
endif()

find_package(SDL3 3.4 REQUIRED CONFIG)

if(WIN32)
    # Windows GUI application (no console window) with the player icon.
    add_executable(mario-sokoban WIN32 main.cpp bitmap_font.hpp icone.rc)
else()
    add_executable(mario-sokoban main.cpp bitmap_font.hpp)
endif()
set_target_properties(mario-sokoban PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/package"
)
target_link_libraries(mario-sokoban PRIVATE SDL3::SDL3)
if(MSVC)
    # The sources are UTF-8; also silence the CRT "unsafe function" warnings.
    target_compile_options(mario-sokoban PRIVATE /utf-8)
    target_compile_definitions(mario-sokoban PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Do not require CLion's MinGW bin directory to be present on PATH at run time.
    target_link_options(mario-sokoban PRIVATE -static-libgcc -static-libstdc++)
endif()

set(PACKAGE_DIR "$<TARGET_FILE_DIR:mario-sokoban>")
if(WIN32)
    add_custom_command(TARGET mario-sokoban POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:mario-sokoban> "${PACKAGE_DIR}"
        COMMAND_EXPAND_LISTS
    )
endif()
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    get_filename_component(MINGW_RUNTIME_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    # Some MinGW distributions link winpthread statically: copy it only if present.
    if(EXISTS "${MINGW_RUNTIME_DIR}/libwinpthread-1.dll")
        add_custom_command(TARGET mario-sokoban POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_RUNTIME_DIR}/libwinpthread-1.dll" "${PACKAGE_DIR}"
        )
    endif()
endif()
add_custom_command(TARGET mario-sokoban POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/sprites_mario_sokoban" "${PACKAGE_DIR}/sprites_mario_sokoban"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/niveaux perso" "${PACKAGE_DIR}/niveaux perso"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_DIR}/data"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/caisse.bmp" "${PACKAGE_DIR}/caisse.bmp"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/REGLES.md" "${PACKAGE_DIR}/REGLES.md"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSES.md" "${PACKAGE_DIR}/LICENSES.md"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/SDL3-zlib.txt" "${PACKAGE_DIR}/SDL3-zlib.txt"
)
