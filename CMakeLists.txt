cmake_minimum_required(VERSION 3.20)
project(MarioSokoban LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        get_filename_component(MINGW_COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        find_program(MINGW_WINDRES_EXECUTABLE NAMES windres HINTS "${MINGW_COMPILER_DIR}" NO_DEFAULT_PATH)
        if(MINGW_WINDRES_EXECUTABLE AND (NOT CMAKE_RC_COMPILER OR NOT IS_ABSOLUTE "${CMAKE_RC_COMPILER}"))
            set(CMAKE_RC_COMPILER "${MINGW_WINDRES_EXECUTABLE}" CACHE FILEPATH "MinGW resource compiler" FORCE)
        endif()
    endif()
    enable_language(RC)
    set(SDL12_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL-1.2")
    add_executable(mario-sokoban WIN32
        main.cpp
        icone.rc
        "${SDL12_SOURCE_DIR}/src/main/win32/SDL_win32_main.c"
    )
else()
    add_executable(mario-sokoban main.cpp)
endif()
set_target_properties(mario-sokoban PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/package")
target_include_directories(mario-sokoban PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL-1.2/include")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(mario-sokoban PRIVATE -finput-charset=ISO-8859-1)
    if(WIN32)
        # CLion's MinGW runtime is not on PATH when the packaged executable is
        # launched, so embed libstdc++ and libgcc in the executable.
        target_link_options(mario-sokoban PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()

if(WIN32)
    # Build the vendored upstream SDL 1.2 Windows sources from its Visual C++
    # source manifest. The manifest lists portable C sources and platform
    # backends; the one MSVC-only MMX translation unit is intentionally omitted.
    set(SDL12_PROJECT_DIR "${SDL12_SOURCE_DIR}/VisualC/SDL")
    file(READ "${SDL12_PROJECT_DIR}/SDL.vcxproj" SDL12_VCXPROJ)
    string(REGEX MATCHALL "<ClCompile Include=\"[^\"]+\"" SDL12_COMPILE_ITEMS "${SDL12_VCXPROJ}")
    set(SDL12_SOURCES)
    foreach(item IN LISTS SDL12_COMPILE_ITEMS)
        string(REGEX REPLACE "^<ClCompile Include=\"([^\"]+)\"$" "\\1" relative_source "${item}")
        file(TO_CMAKE_PATH "${relative_source}" relative_source)
        get_filename_component(source_file "${relative_source}" ABSOLUTE BASE_DIR "${SDL12_PROJECT_DIR}")
        if(EXISTS "${source_file}" AND NOT source_file MATCHES "SDL_mixer_MMX_VC\\.c$")
            list(APPEND SDL12_SOURCES "${source_file}")
        endif()
    endforeach()
    if(NOT SDL12_SOURCES)
        message(FATAL_ERROR "No SDL 1.2 source files were found in the vendored VisualC project.")
    endif()

    set(SDL12_GENERATED_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}/sdl12-generated")
    file(MAKE_DIRECTORY "${SDL12_GENERATED_INCLUDE}")
    configure_file("${SDL12_SOURCE_DIR}/include/SDL_config_win32.h"
                   "${SDL12_GENERATED_INCLUDE}/SDL_config.h" COPYONLY)
    add_library(SDL12 SHARED ${SDL12_SOURCES})
    set_target_properties(SDL12 PROPERTIES PREFIX "" OUTPUT_NAME "SDL" WINDOWS_EXPORT_ALL_SYMBOLS ON)
    target_include_directories(SDL12 BEFORE PUBLIC
        "${SDL12_GENERATED_INCLUDE}"
        "${SDL12_SOURCE_DIR}/include"
    )
    target_compile_definitions(SDL12 PRIVATE __WIN32__ BUILD_SDL)
    target_link_libraries(SDL12 PRIVATE
        ddraw dxguid gdi32 imm32 ole32 shell32 user32 winmm
        opengl32 version
    )
    target_link_libraries(mario-sokoban PRIVATE SDL12 windowscodecs ole32)
    add_custom_command(TARGET mario-sokoban POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:SDL12>" "$<TARGET_FILE_DIR:mario-sokoban>"
    )
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        get_filename_component(MINGW_RUNTIME_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        # Some MinGW distributions link winpthread statically: copy it only if present.
        if(EXISTS "${MINGW_RUNTIME_DIR}/libwinpthread-1.dll")
            add_custom_command(TARGET mario-sokoban POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_RUNTIME_DIR}/libwinpthread-1.dll" "$<TARGET_FILE_DIR:mario-sokoban>"
            )
        endif()
    endif()
else()
    find_path(SDL12_INCLUDE_DIR NAMES SDL/SDL.h REQUIRED)
    find_library(SDL12_LIBRARY NAMES SDL SDL-1.2 REQUIRED)
    target_include_directories(mario-sokoban PRIVATE "${SDL12_INCLUDE_DIR}" "${SDL12_INCLUDE_DIR}/SDL")
    target_link_libraries(mario-sokoban PRIVATE "${SDL12_LIBRARY}")
endif()

set(PACKAGE_DIR "$<TARGET_FILE_DIR:mario-sokoban>")
add_custom_command(TARGET mario-sokoban POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/sprites_mario_sokoban" "${PACKAGE_DIR}/sprites_mario_sokoban"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/niveaux perso" "${PACKAGE_DIR}/niveaux perso"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data" "${PACKAGE_DIR}/data"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/caisse.bmp" "${PACKAGE_DIR}/caisse.bmp"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/caisse.ico" "${PACKAGE_DIR}/caisse.ico"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/REGLES.md" "${PACKAGE_DIR}/REGLES.md"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSES.md" "${PACKAGE_DIR}/LICENSES.md"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/SDL-1.2-LGPL-2.1.txt" "${PACKAGE_DIR}/SDL-1.2-LGPL-2.1.txt"
)
