add_subdirectory(console)
add_subdirectory(hex_editor)
add_subdirectory(text_editor)
add_subdirectory(machine_editor)

add_library(
        ide
        console
        hex_editor
        text_editor
        machine_editor
        context.cpp context.h
        environment.cpp environment.h)
target_link_libraries(
        ide
        console
        hex_editor
        text_editor
        machine_editor)