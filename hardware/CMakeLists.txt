add_library(
        hardware
        hardware.h
        ram.cpp ram.h
        rom.cpp rom.h
        hardware_types.h
        machine.cpp machine.h
        display.cpp display.h
        registry.cpp registry.h
        hardware.h hardware.cpp
        component.cpp component.h
        memory_map.cpp memory_map.h
        dip_switch.cpp dip_switch.h
        memory_mapper.cpp memory_mapper.h
        cpus/mc6809/cpu.cpp cpus/mc6809/cpu.h
        memory_map_entry.cpp memory_map_entry.h
        cpus/mc6809/opcode.cpp cpus/mc6809/opcode.h
        integrated_circuit.cpp integrated_circuit.h
        video/midway_mcr_video_generator.cpp video/midway_mcr_video_generator.h
        cpus/mc6809/assembly_language_parser.cpp cpus/mc6809/assembly_language_parser.h)
