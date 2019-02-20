cmake_minimum_required (VERSION 3.11)
project (ryu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-private-field -Wno-unknown-pragmas -Wno-inconsistent-missing-override" CACHE STRING "compile flags" FORCE)
    message(STATUS "Using clang flags: ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-lstdc++fs -Wall" CACHE STRING "compile flags" FORCE)
    message(STATUS "Using gcc flags: ${CMAKE_CXX_FLAGS}")
else ()
    message(STATUS "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# dummy target used for file copies
add_custom_target(dummy-target ALL DEPENDS custom-output)
add_custom_command(OUTPUT custom-output COMMAND ${CMAKE_COMMAND} -E echo DEPENDS always-rebuild)
add_custom_command(OUTPUT always-rebuild COMMAND ${CMAKE_COMMAND} -E echo)

# libfmt
add_subdirectory(ext/fmt-5.3.0 EXCLUDE_FROM_ALL)
include_directories(ext/fmt-5.3.0/include)

# dyncall
add_subdirectory(ext/dyncall-1.0 EXCLUDE_FROM_ALL)
include_directories(ext/dyncall-1.0)

# boost
include_directories(ext/boost-1.69.0)

if (WIN32)
    add_definitions("-DBOOST_ALL_NO_LIB")
endif()

# boost system library
add_library(boost-system ext/boost-1.69.0/libs/system/src/error_code.cpp)

# boost filesystem library
set(
    BOOST_FILESYSTEM_SOURCES
    ext/boost-1.69.0/libs/filesystem/src/codecvt_error_category.cpp
    ext/boost-1.69.0/libs/filesystem/src/operations.cpp
    ext/boost-1.69.0/libs/filesystem/src/path.cpp
    ext/boost-1.69.0/libs/filesystem/src/path_traits.cpp
    ext/boost-1.69.0/libs/filesystem/src/portability.cpp
    ext/boost-1.69.0/libs/filesystem/src/unique_path.cpp
    ext/boost-1.69.0/libs/filesystem/src/utf8_codecvt_facet.cpp
)
if (WIN32)
    set(
        BOOST_FILESYSTEM_SOURCES
        ${BOOST_FILESYSTEM_SOURCES}
        ext/boost-1.69.0/libs/filesystem/src/windows_file_codecvt.cpp
    )
endif()
add_library(boost-filesystem ${BOOST_FILESYSTEM_SOURCES})

# yaml-cpp
option(YAML_CPP_BUILD_TESTS "" OFF)
option(YAML_CPP_BUILD_TOOLS "" OFF)
add_subdirectory(ext/yaml-cpp-0.6.2)
include_directories(ext/yaml-cpp-0.6.2/include)

# freetype
add_subdirectory(ext/freetype-2.9)
include_directories(ext/freetype-2.9/include)

# sdl2
set(SDL_SHARED OFF)
add_subdirectory(ext/SDL2-2.0.9)
include_directories(ext/SDL2-2.0.9/include)

# sdl2_ttf
add_subdirectory(ext/SDL2_ttf-2.0.14)
include_directories(ext/SDL2_ttf-2.0.14)

# sdl2_image
# sdl2_mixer

# catch2
include_directories(ext/catch2-2.6.1/include)

# SDL_FontCache
add_library(
    SDL_FontCache
    ext/SDL_FontCache/SDL_FontCache.c
    ext/SDL_FontCache/SDL_FontCache.h)
include_directories(ext/SDL_FontCache)

# entt
include_directories(ext/entt-3.0.0)

#
include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
)

# target & linkage
add_executable(
    ryu
    assets
    LICENSE
    main.cpp
    README.md
    .gitignore
)

target_link_libraries(
    ryu
    yaml-cpp
    boost-system
    boost-filesystem
    fmt::fmt
    SDL_FontCache
    SDL2_ttf
    freetype
    SDL2-static
)

add_custom_target(ryu-configured DEPENDS dummy-target ryu)
add_custom_command(
    TARGET ryu-configured
    COMMAND ${CMAKE_COMMAND} -E echo  "make_directory: ${PROJECT_BINARY_DIR}/bin/logs"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin/logs

    COMMAND ${CMAKE_COMMAND} -E echo "copy_directory: ${PROJECT_SOURCE_DIR}/assets to ${PROJECT_BINARY_DIR}/bin/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/bin/assets
)
