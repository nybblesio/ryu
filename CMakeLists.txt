cmake_minimum_required (VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Wno-unknown-pragmas -Wno-inconsistent-missing-override")

project (ryu)
set (ryu_version_major 1)
set (ryu_version_minor 0)
configure_file (
    "${PROJECT_SOURCE_DIR}/ryu_config.in.h"
    "${PROJECT_BINARY_DIR}/ryu_config.h"
)

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

# XXX: this doesn't seem correct but it works for now
get_target_property(RTTR_INCLUDE_DIR RTTR::Core INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS ${RTTR_INCLUDE_DIR})

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
        ${RTTR_INCLUDE_DIR}
)

# project libraries
add_subdirectory(ide)
add_subdirectory(cpu)
add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(machine)
add_subdirectory(hardware)
add_subdirectory(emulator)

# target & linkage
add_executable(
        ryu
        ide
        assets
        common
        emulator
        hardware
        machine
        core
        ide
        cpu
        LICENSE
        README.md
        .gitignore
        main.cpp
        ryu_types.h
        ryu.properties
        ryu_config.in.h
        application.cpp application.h)
target_link_libraries(
        ryu
        ide
        emulator
        machine
        hardware
        cpu
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
add_custom_command(
        TARGET ryu-configured
        COMMAND ${CMAKE_COMMAND} -E echo "make_directory: ${PROJECT_BINARY_DIR}/logs"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/logs

        COMMAND ${CMAKE_COMMAND} -E echo "copy: ${PROJECT_SOURCE_DIR}/ryu.properties"
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/ryu.properties ${PROJECT_BINARY_DIR}

        COMMAND ${CMAKE_COMMAND} -E echo "copy_directory: ${PROJECT_SOURCE_DIR}/assets to ${PROJECT_BINARY_DIR}/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/assets
)
