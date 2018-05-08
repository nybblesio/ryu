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

#include <core/views/panel.h>
#include "layout_engine.h"
#include "view.h"

namespace ryu::core {

    layout_engine::layout_engine() {
        lay_init_context(&_context);
    }

    layout_engine::~layout_engine() {
        lay_destroy_context(&_context);
    }

    void layout_engine::reset() {
        lay_reset_context(&_context);
        _view_to_rect.clear();
    }

    void layout_engine::size(const core::size& value) {
        _root_size = value;
    }

    core::rect layout_engine::view_rect(core::view* v) {
        auto it = _view_to_rect.find(v->id());
        if (it == _view_to_rect.end())
            return {};

        lay_vec4 lay_rect = lay_get_rect(&_context, it->second);
        return {lay_rect[0], lay_rect[1], lay_rect[2], lay_rect[3]};
    }

    void layout_engine::calculate(core::view* root_view) {
        reset();
        build_layout_tree(root_view, -1);
        lay_run_context(&_context);
    }

    void layout_engine::build_layout_tree(core::view* v, int32_t parent_lay_id) {
        lay_id current_lay_id = lay_item(&_context);
        if (parent_lay_id > -1) {
            lay_insert(&_context, static_cast<lay_id>(parent_lay_id), current_lay_id);
            auto& min_size = v->min_size();
            lay_set_size_xy(
                &_context,
                current_lay_id,
                static_cast<lay_scalar>(min_size.width()),
                static_cast<lay_scalar>(min_size.height()));
        } else {
            lay_set_size_xy(
                &_context,
                current_lay_id,
                static_cast<lay_scalar>(_root_size.width()),
                static_cast<lay_scalar>(_root_size.height()));
        }

        auto& margins = v->margin();
        lay_set_margins_ltrb(
            &_context,
            current_lay_id,
            static_cast<lay_scalar>(margins.left()),
            static_cast<lay_scalar>(margins.top()),
            static_cast<lay_scalar>(margins.right()),
            static_cast<lay_scalar>(margins.bottom()));

        uint32_t behavior = 0;
        switch (v->dock()) {
            case dock::none:
                break;
            case dock::bottom:
                behavior |= LAY_BOTTOM;
                break;
            case dock::fill:
                behavior |= LAY_FILL;
                break;
            case dock::left:
                behavior |= LAY_LEFT;
                break;
            case dock::right:
                behavior |= LAY_RIGHT;
                break;
            case dock::top:
                behavior |= LAY_TOP;
                break;
        }

        if (v->type() == view::types::container) {
            uint32_t flags = 0;

            auto panel = dynamic_cast<core::panel*>(v);
            if (panel != nullptr) {
                switch (panel->layout_mode()) {
                    case panel::layout_modes::flow:
                        flags |= LAY_LAYOUT;
                        break;
                    case panel::layout_modes::flex:
                        flags |= LAY_FLEX;
                        break;
                }

                switch (panel->flex_direction()) {
                    case panel::flex_directions::none:
                        break;
                    case panel::flex_directions::row:
                        flags |= LAY_ROW;
                        behavior |= LAY_HFILL;
                        break;
                    case panel::flex_directions::column:
                        flags |= LAY_COLUMN;
                        behavior |= LAY_VFILL;
                        break;
                }

                switch (panel->layout_justification()) {
                    case panel::layout_justifications::start:
                        flags |= LAY_START;
                        break;
                    case panel::layout_justifications::middle:
                        flags |= LAY_MIDDLE;
                        break;
                    case panel::layout_justifications::end:
                        flags |= LAY_END;
                        break;
                    case panel::layout_justifications::full:
                        flags |= LAY_JUSTIFY;
                        break;
                }

                if (panel->layout_wrap())
                    flags |= LAY_WRAP;
            }

            lay_set_contain(&_context, current_lay_id, flags);
        }

        lay_set_behave(&_context, current_lay_id, behavior);

        _view_to_rect.insert(std::make_pair(v->id(), current_lay_id));

        for (auto child : v->children())
            build_layout_tree(child, current_lay_id);
    }

}