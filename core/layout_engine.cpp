//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#define LAY_IMPLEMENTATION
#include "layout_engine.h"

namespace ryu::core {

    layout_engine::layout_engine() {
        lay_init_context(&_context);
        _root = lay_item(&_context);
    }

    layout_engine::~layout_engine() {
        lay_destroy_context(&_context);
    }

    void layout_engine::reset() {
        lay_reset_context(&_context);
    }

    void layout_engine::size(const core::size& value) {
        lay_set_size_xy(
            &_context,
            _root,
            static_cast<lay_scalar>(value.width()),
            static_cast<lay_scalar>(value.height()));
    }

}