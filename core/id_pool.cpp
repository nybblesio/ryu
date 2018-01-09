//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "id_pool.h"

namespace ryu::core {

    id_pool::id_pool() {
        _pool.insert(id_interval(1, std::numeric_limits<int32_t>::max()));
    }

    int32_t id_pool::allocate() {
        id_interval first = *(_pool.begin());
        int32_t id = first.left();
        _pool.erase(_pool.begin());
        if (first.left() + 1 <= first.right()) {
            _pool.insert(id_interval(first.left() + 1 , first.right()));
        }
        return id;
    }

    id_pool* id_pool::instance() {
        static id_pool pool;
        return &pool;
    }

    void id_pool::release(int32_t id) {
        auto it = _pool.find(id_interval(id, id));
        if (it != _pool.end() && it->left() <= id && it->right() > id) {
            return;
        }
        it = _pool.upper_bound(id_interval(id, id));
        if (it == _pool.end()) {
            return;
        } else {
            id_interval _interval = *(it);
            if (id + 1 != _interval.left()) {
                _pool.insert(id_interval(id, id));
            } else {
                if (it != _pool.begin()) {
                    auto it_2 = it;
                    --it_2;
                    if (it_2->right() + 1 == id) {
                        id_interval _interval2 = *(it_2);
                        _pool.erase(it);
                        _pool.erase(it_2);
                        _pool.insert(
                                id_interval(_interval2.left(),
                                            _interval.right()));
                    } else {
                        _pool.erase(it);
                        _pool.insert(id_interval(id, _interval.right()));
                    }
                } else {
                    _pool.erase(it);
                    _pool.insert(id_interval(id, _interval.right()));
                }
            }
        }
    }

    bool id_pool::mark_used(int32_t id) {
        auto it = _pool.find(id_interval(id,id));
        if (it == _pool.end()) {
            return false;
        } else {
            id_interval free_interval = *(it);
            _pool.erase (it);
            if (free_interval.left() < id) {
                _pool.insert(id_interval(free_interval.left(), id-1));
            }
            if (id +1 <= free_interval.right() ) {
                _pool.insert(id_interval(id+1, free_interval.right()));
            }
            return true;
        }
    }

    bool id_pool::mark_range(int32_t start_id, int32_t end_id) {
        for (size_t id = static_cast<size_t>(start_id);
             id < static_cast<size_t>(end_id);
             ++id) {
            auto success = mark_used(static_cast<int32_t>(id));
            if (!success)
                return false;
        }
        return true;
    }

    // XXX: this feels wrong, need to rework this
    int32_t id_pool::allocate_from_range(int32_t start_id, int32_t end_id) {
        for (size_t id = static_cast<size_t>(start_id);
             id < static_cast<size_t>(end_id);
             ++id) {
            auto success = mark_used(static_cast<int32_t>(id));
            if (success)
                return static_cast<int32_t>(id);
        }
        return -1;
    }

}
