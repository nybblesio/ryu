//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <map>
#include <vector>
#include <core/id_pool.h>
#include <common/IntervalTree.h>

namespace ryu::hardware {

    enum class meta_data_key {
        type_id,
        type_name
    };

    struct integrated_circuit_type_t {
        uint16_t id {};
        std::string name {};
    };

    typedef std::vector<integrated_circuit_type_t> ic_type_list;

    static constexpr uint16_t integrated_circuit_id = 1;
    static constexpr uint16_t memory_mapper_id = 2;
    static constexpr uint16_t ram_id = 3;
    static constexpr uint16_t rom_id = 4;

    class memory;

    class machine;

    typedef std::map<uint32_t, machine> machine_dict;
    typedef std::vector<machine*> machine_list;

    class display;

    class memory_mapper;

    class memory_map_entry;

    typedef std::vector<Interval<memory_map_entry*>> mm_entry_interval_list;

    class integrated_circuit;

    typedef std::vector<Interval<integrated_circuit*>> ic_interval_list;
    typedef std::vector<integrated_circuit*> ic_list;

    class component;

    typedef std::vector<component*> component_list;
    typedef std::map<uint32_t, component*> component_dict;

    typedef std::vector<display> display_list;
};