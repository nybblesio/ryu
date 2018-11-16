cmake_minimum_required (VERSION 3.11)
project (ryu)
include(ExternalProject)

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

set (ryu_version_major 1)
set (ryu_version_minor 0)
configure_file (
    "${PROJECT_SOURCE_DIR}/ryu_config.in.h"
    "${PROJECT_BINARY_DIR}/ryu_config.h"
)

# enhance unit test discovery
enable_testing()
set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS "")
macro(add_unit_test target test_name args)
    set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${CMAKE_CURRENT_BINARY_DIR}/${target})
    add_test(NAME ${test_name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target} ${args})
endmacro(add_unit_test)

# dummy target used for file copies
add_custom_target(dummy-target ALL DEPENDS custom-output)
add_custom_command(OUTPUT custom-output COMMAND ${CMAKE_COMMAND} -E echo DEPENDS always-rebuild)
add_custom_command(OUTPUT always-rebuild COMMAND ${CMAKE_COMMAND} -E echo)

# libfmt
add_subdirectory(ext/fmt-5.1.0 EXCLUDE_FROM_ALL)
include_directories(ext/fmt-5.1.0/include)

# dyncall
add_subdirectory(ext/dyncall-1.0 EXCLUDE_FROM_ALL)
include_directories(ext/dyncall-1.0)

# boost
include_directories(ext/boost-1.67.0)

if (WIN32)
    add_definitions("-DBOOST_ALL_NO_LIB")
endif()

# boost system library
add_library(boost-system ext/boost-1.67.0/libs/system/src/error_code.cpp)

# boost filesystem library
set(
    BOOST_FILESYSTEM_SOURCES
    ext/boost-1.67.0/libs/filesystem/src/codecvt_error_category.cpp
    ext/boost-1.67.0/libs/filesystem/src/operations.cpp
    ext/boost-1.67.0/libs/filesystem/src/path.cpp
    ext/boost-1.67.0/libs/filesystem/src/path_traits.cpp
    ext/boost-1.67.0/libs/filesystem/src/portability.cpp
    ext/boost-1.67.0/libs/filesystem/src/unique_path.cpp
    ext/boost-1.67.0/libs/filesystem/src/utf8_codecvt_facet.cpp
)
if (WIN32)
    set(
        BOOST_FILESYSTEM_SOURCES
        ${BOOST_FILESYSTEM_SOURCES}
        ext/boost-1.67.0/libs/filesystem/src/windows_file_codecvt.cpp
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
include_directories(ext/catch2-2.3.0/include)

# rttr
option(BUILD_EXAMPLES "" OFF)
option(BUILD_UNIT_TESTS "" OFF)
option(BUILD_DOCUMENTATION "" OFF)
add_subdirectory(ext/rttr-0.9.7)
include_directories(
    ext/rttr-0.9.7/src
    ${PROJECT_BINARY_DIR}/ext/rttr-0.9.7/src)

# SDL_FontCache
add_library(
    SDL_FontCache
    ext/SDL_FontCache/SDL_FontCache.c
    ext/SDL_FontCache/SDL_FontCache.h)
include_directories(ext/SDL_FontCache)

#
include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
)

# project libraries
add_subdirectory(ide)
add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(hardware)
add_subdirectory(emulator)

# all_unit_tests target
get_property(test_targets GLOBAL PROPERTY UNIT_TEST_TARGETS)
message(STATUS "UNIT_TEST_TARGETS=${test_targets}")
add_custom_target(all-unit-tests ALL DEPENDS ${test_targets})
add_custom_command(
    TARGET all-unit-tests
    COMMENT "Execute all unit tests"
    POST_BUILD
    COMMAND ctest -C $<CONFIGURATION> --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# target & linkage
add_executable(
    ryu
    ide
    assets
    emulator
    hardware
    core
    ide
    common
    LICENSE
    todo.txt
    main.cpp
    README.md
    .gitignore
    ryu_types.h
    machines.yaml
    ryu_config.in.h
    ext/layout/layout.h
    application.cpp application.h
    logger_factory.cpp logger_factory.h
)
target_link_libraries(
    ryu
    ide
    emulator
    hardware
    core
    common
    yaml-cpp
    boost-system
    boost-filesystem
    fmt::fmt
    SDL2-static
    SDL_FontCache
    SDL2_ttf
    freetype
    RTTR::Core
)

add_custom_target(ryu-configured DEPENDS dummy-target ryu)
add_dependencies(ryu-configured all-unit-tests)
add_custom_command(
    TARGET ryu-configured
    COMMAND ${CMAKE_COMMAND} -E echo  "make_directory: ${PROJECT_BINARY_DIR}/bin/logs"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin/logs

    COMMAND ${CMAKE_COMMAND} -E echo "copy: ${PROJECT_SOURCE_DIR}/machines.yaml"
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/machines.yaml ${PROJECT_BINARY_DIR}/bin

    COMMAND ${CMAKE_COMMAND} -E echo "copy_directory: ${PROJECT_SOURCE_DIR}/assets to ${PROJECT_BINARY_DIR}/bin/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/bin/assets
)
