add_subdirectory(hex_editor)
add_subdirectory(console_editor)
add_subdirectory(source_editor)
add_subdirectory(machine_editor)

add_library(
        ide
        hex_editor
        source_editor
        console_editor
        machine_editor
        ide_types.h
        ide_context.cpp ide_context.h)
target_link_libraries(
        ide
        hex_editor
        console_editor
        source_editor
        machine_editor)