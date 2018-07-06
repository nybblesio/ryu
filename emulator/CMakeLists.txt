project(emulator)

add_library(
        ${PROJECT_NAME}
        controller.cpp controller.h
        emulator_context.cpp emulator_context.h)