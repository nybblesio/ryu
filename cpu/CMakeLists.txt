add_subdirectory(mc6809)

add_library(
        cpu
        mc6809
        registry.cpp registry.h model.cpp model.h types.h)
target_link_libraries(
        cpu
        mc6809
)