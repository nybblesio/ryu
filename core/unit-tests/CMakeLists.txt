file(GLOB UNIT_TEST_SOURCES *.cpp *.h)

enable_testing()

add_executable(
        core-unit-tests
        ${UNIT_TEST_SOURCES}
        ${PROJECT_SOURCE_DIR}/catch2.hpp)

target_link_libraries(
        core-unit-tests
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

add_unit_test(core-unit-tests document-tests "[document]")
add_unit_test(core-unit-tests piece-table-tests "[piece-table]")