add_subdirectory(hex_editor)
add_subdirectory(console_editor)
add_subdirectory(component_editor)
add_subdirectory(source_editor)
add_subdirectory(machine_list)
add_subdirectory(machine_editor)
add_subdirectory(project_editor)

add_library(
        ide
        hex_editor
        source_editor
        console_editor
        component_editor
        project_editor
        machine_list
        machine_editor
        ide_types.h
        ide_context.cpp ide_context.h)
target_link_libraries(
        ide
        hex_editor
        console_editor
        component_editor
        source_editor
        machine_list
        machine_editor
        project_editor)