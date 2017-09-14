add_library(
        hardware
        memory.h
        cpu.cpp cpu.h
        ram.cpp ram.h
        rom.cpp rom.h
        hardware_types.h
        machine.cpp machine.h
        display.cpp display.h
        component.cpp component.h
        memory_map.cpp memory_map.h
        memory_mapper.cpp memory_mapper.h
        memory_map_entry.cpp memory_map_entry.h
        integrated_circuit.cpp integrated_circuit.h)
