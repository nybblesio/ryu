//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <map>
#include <vector>
#include <common/IntervalTree.h>

namespace ryu::hardware {

    class memory;

    class machine;

    typedef std::map<int, machine> machine_dict;
    typedef std::vector<machine*> machine_list;

    class display;

    class memory_mapper;

    class memory_map_entry;

    typedef std::vector<Interval<memory_map_entry*>> mm_entry_interval_list;

    class integrated_circuit;

    typedef std::vector<Interval<integrated_circuit*>> ic_interval_list;

    class component;

    typedef std::vector<component*> component_list;
    typedef std::map<int, component*> component_dict;

    typedef std::vector<display> display_list;
};