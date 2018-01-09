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

namespace ryu::core {

    class id_interval {
    public:
        id_interval(int ll, int uu) : _value(ll, uu) {}

        bool operator<(const id_interval& s) const {
            return
                    (_value.lower() < s._value.lower()) &&
                    (_value.upper() < s._value.lower());
        }

        int left() const { return _value.lower(); }

        int right() const { return _value.upper(); }

    private:
        boost::numeric::interval<int> _value;
    };

    typedef std::set<id_interval> id_set;

    class id_pool {
    public:
        static id_pool* instance();

        int32_t allocate();

        void release(int32_t id);

        bool mark_used(int32_t id);

        bool mark_range(int32_t start_id, int32_t end_id);

        int32_t allocate_from_range(int32_t start_id, int32_t end_id);

    protected:
        id_pool();

    private:
        id_set _pool;
    };

};

