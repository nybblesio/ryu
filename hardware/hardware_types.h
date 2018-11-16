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
        type_name,
        type_category
    };

    struct integrated_circuit_type_t {
        uint16_t id {};
        std::string name {};
    };

    using ic_type_list = std::vector<integrated_circuit_type_t>;

    static constexpr uint16_t category_circuit = 0;
    static constexpr uint16_t category_memory = 1;
    static constexpr uint16_t category_cpu = 2;
    static constexpr uint16_t category_video = 3;
    static constexpr uint16_t category_audio = 4;
    static constexpr uint16_t category_input = 5;

    static constexpr uint16_t integrated_circuit_id = 1;
    static constexpr uint16_t memory_mapper_id = 2;
    static constexpr uint16_t ram_id = 3;
    static constexpr uint16_t rom_id = 4;
    static constexpr uint16_t digital_audio_id = 5;
    static constexpr uint16_t fm_synthesizer_id = 6;
    static constexpr uint16_t video_generator_id = 7;
    static constexpr uint16_t cpu_id = 10;
    static constexpr uint16_t dip_switch_id = 11;

    class memory;

    class machine;

    using machine_dict = std::map<uint32_t, machine>;
    using machine_list = std::vector<machine*>;

    class display;

    class memory_mapper;

    class memory_map_entry;

    using mm_entry_interval_list = std::vector<Interval<memory_map_entry*>>;

    class integrated_circuit;

    using ic_interval = Interval<integrated_circuit*>;
    using ic_interval_list = std::vector<ic_interval>;
    using ic_list = std::vector<integrated_circuit*>;

    class component;

    using component_list = std::vector<component*>;
    using component_dict = std::map<uint32_t, component*>;

    using display_list = std::vector<display>;
};