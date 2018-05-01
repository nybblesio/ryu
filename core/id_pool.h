//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#pragma once

#include <boost/numeric/interval.hpp>
#include <set>
#include "core_types.h"

namespace ryu::core {

    class id_interval {
    public:
        id_interval(id_t ll, id_t uu) : _value(ll, uu) {}

        bool operator<(const id_interval& s) const {
            return
                    (_value.lower() < s._value.lower()) &&
                    (_value.upper() < s._value.lower());
        }

        id_t left() const { return _value.lower(); }

        id_t right() const { return _value.upper(); }

    private:
        boost::numeric::interval<id_t> _value;
    };

    typedef std::set<id_interval> id_set;

    class id_pool {
    public:
        static id_pool* instance();

        id_t allocate();

        void release(id_t id);

        bool mark_used(id_t id);

        bool mark_range(id_t start_id, id_t end_id);

        id_t allocate_from_range(id_t start_id, id_t end_id);

    protected:
        id_pool();

    private:
        id_set _pool;
    };

};

