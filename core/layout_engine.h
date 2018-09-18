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

#include <map>
#include <ext/layout/layout.h>
#include "size.h"
#include "rect.h"

namespace ryu::core {

    class view;
    class panel;

    class layout_engine {
    public:
        layout_engine();

        virtual ~layout_engine();

        void reset();

        void size(const core::size& value);

        core::rect view_rect(core::view* v);

        void calculate(core::view* root_view);

    private:
        void build_layout_tree(core::view* v, int32_t parent_lay_id);

        std::pair<uint32_t, uint32_t> get_layout_flags(core::view* view);

    private:
        lay_context _context;
        core::size _root_size {0, 0};
        std::map<uint32_t, lay_id> _view_to_rect {};
    };

};
