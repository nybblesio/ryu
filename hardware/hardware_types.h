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

    class cpu;

    class memory;

    class memory_map;

    class machine;

    class display;

    class integrated_circuit;

    typedef std::vector<Interval<integrated_circuit*>> ic_interval_list;

    class component;

    typedef std::vector<component*> component_list;
    typedef std::map<int, component*> component_dict;
};