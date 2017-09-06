add_subdirectory(console)
add_subdirectory(hex_editor)
add_subdirectory(text_editor)

add_library(
        ide
        console
        hex_editor
        text_editor
        context.cpp context.h
        command_factory.cpp command_factory.h)
target_link_libraries(
        ide
        console
        hex_editor
        text_editor)