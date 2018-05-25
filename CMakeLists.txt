cmake_minimum_required (VERSION 3.11)
project (ryu)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-private-field -Wno-unknown-pragmas -Wno-inconsistent-missing-override" CACHE STRING "compile flags" FORCE)
    message(STATUS "Using clang flags: ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-lstdc++fs -Wall" CACHE STRING "compile flags" FORCE)
    message(STATUS "Using gcc flags: ${CMAKE_CXX_FLAGS}")
else ()
    message(STATUS "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# rpath setup
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "/usr/local/lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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

# conan configuration
set(CONAN_DISABLE_CHECK_COMPILER true)
include(conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS true)

# 3rd party dependencies
find_package(Boost 1.65.0 COMPONENTS system filesystem REQUIRED)
find_package(Log4cpp REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(RTTR CONFIG REQUIRED Core)

include_directories(
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${Log4cpp_INCLUDE_DIR}
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
        layout.h
        README.md
        .gitignore
        ryu_types.h
        machines.yaml
        ryu.properties
        ryu_config.in.h
        taking_break.txt
        application.cpp application.h
        logger_factory.cpp logger_factory.h)

target_link_libraries(
        ryu
        ide
        emulator
        hardware
        core
        common
        ${CONAN_LIBS}
        ${Boost_LIBRARIES}
        ${LOG4CPP_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        RTTR::Core
)

add_custom_target(ryu-configured DEPENDS dummy-target ryu)
add_dependencies(ryu-configured all-unit-tests)
add_custom_command(
        TARGET ryu-configured
        COMMAND ${CMAKE_COMMAND} -E echo "make_directory: ${PROJECT_BINARY_DIR}/bin/logs"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin/logs

        COMMAND ${CMAKE_COMMAND} -E echo "copy: ${PROJECT_SOURCE_DIR}/ryu.properties"
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/ryu.properties ${PROJECT_BINARY_DIR}/bin

        COMMAND ${CMAKE_COMMAND} -E echo "copy: ${PROJECT_SOURCE_DIR}/machines.yaml"
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/machines.yaml ${PROJECT_BINARY_DIR}/bin

        COMMAND ${CMAKE_COMMAND} -E echo "copy_directory: ${PROJECT_SOURCE_DIR}/assets to ${PROJECT_BINARY_DIR}/bin/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/bin/assets
)
