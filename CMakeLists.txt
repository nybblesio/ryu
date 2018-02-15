cmake_minimum_required (VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-private-field -Wno-unknown-pragmas -Wno-inconsistent-missing-override" CACHE STRING "compile flags" FORCE)
message(STATUS ${CMAKE_CXX_FLAGS})

project (ryu)
set (ryu_version_major 1)
set (ryu_version_minor 0)
configure_file (
    "${PROJECT_SOURCE_DIR}/ryu_config.in.h"
    "${PROJECT_BINARY_DIR}/ryu_config.h"
)

enable_testing()

# enhance unit test discovery
set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS "")

macro(add_unit_test target test_name args)
    set_property(GLOBAL PROPERTY UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${CMAKE_CURRENT_BINARY_DIR}/${target})
    add_test(NAME ${test_name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target} ${args})
endmacro(add_unit_test)

# project cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# dummy target used for file copies
add_custom_target(dummy-target ALL DEPENDS custom-output)
add_custom_command(OUTPUT custom-output COMMAND ${CMAKE_COMMAND} -E echo DEPENDS always-rebuild)
add_custom_command(OUTPUT always-rebuild COMMAND ${CMAKE_COMMAND} -E echo)

# 3rd party dependencies
find_package(Boost 1.65.0 COMPONENTS system filesystem REQUIRED)
find_package(FMT REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Log4cpp REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(RTTR REQUIRED Core)

include_directories(
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${YAML_CPP_INCLUDE_DIR}
        ${FMT_INCLUDE_DIRS}
        ${Log4cpp_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        RTTR::Core
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

# rpath setup
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "/usr/local/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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
        README.md
        todo.txt
        machines.yaml
        .gitignore
        catch2.hpp
        main.cpp
        ryu_types.h
        ryu.properties
        ryu_config.in.h
        taking_break.txt
        application.cpp application.h)

target_link_libraries(
        ryu
        ide
        emulator
        hardware
        core
        common
        ${SDL2_LIBRARY}
        ${Boost_LIBRARIES}
        ${LOG4CPP_LIBRARIES}
        ${YAML_CPP_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        fmt::fmt
         RTTR::Core
)

add_custom_target(ryu-configured DEPENDS dummy-target ryu)
add_dependencies(ryu-configured all-unit-tests)
add_custom_command(
        TARGET ryu-configured
        COMMAND ${CMAKE_COMMAND} -E echo "make_directory: ${PROJECT_BINARY_DIR}/logs"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/logs

        COMMAND ${CMAKE_COMMAND} -E echo "copy: ${PROJECT_SOURCE_DIR}/ryu.properties"
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/ryu.properties ${PROJECT_BINARY_DIR}

        COMMAND ${CMAKE_COMMAND} -E echo "copy: ${PROJECT_SOURCE_DIR}/machines.yaml"
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/machines.yaml ${PROJECT_BINARY_DIR}

        COMMAND ${CMAKE_COMMAND} -E echo "copy_directory: ${PROJECT_SOURCE_DIR}/assets to ${PROJECT_BINARY_DIR}/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/assets
)
