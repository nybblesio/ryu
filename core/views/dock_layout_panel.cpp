//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include <logger_factory.h>
#include "dock_layout_panel.h"

namespace ryu::core {

    static logger* s_log = logger_factory::instance()->create(
            "dock_layout_panel",
            logger::level::info);

    dock_layout_panel::dock_layout_panel(
            const std::string& name,
            core::view_host* host) : panel(name, host) {
    }

    dock_layout_panel::~dock_layout_panel() {
    }

    void dock_layout_panel::resize_child(
            core::view* child,
            core::rect& bounds) {
        auto& rect = child->bounds();
        auto margins = child->margin();

        auto horizontal_margin = margins.left() + margins.right();
        auto width_plus_margin = rect.width() + horizontal_margin;
        auto vertical_margin = margins.top() + margins.bottom();
        auto height_plus_margin = rect.height() + vertical_margin;

        switch (child->dock()) {
            case dock::bottom: {
                rect.pos(bounds.left() + margins.left(), bounds.bottom() - height_plus_margin);
                rect.width(bounds.width() - margins.right());
                bounds.height(bounds.height() - height_plus_margin);
                break;
            }
            case dock::fill: {
                rect = {
                    bounds.left() + margins.left(),
                    bounds.top() + margins.top(),
                    bounds.width() - margins.right(),
                    bounds.height() - margins.bottom()
                };
                break;
            }
            case dock::left: {
                rect.pos(bounds.left() + margins.left(), bounds.top() + margins.top());
                rect.height(bounds.height() - vertical_margin);
                bounds.width(bounds.width() - width_plus_margin);
                bounds.left(bounds.left() + width_plus_margin);
                break;
            }
            case dock::right: {
                rect.pos(bounds.right() - width_plus_margin, bounds.top() + margins.top());
                rect.height(bounds.height() - vertical_margin);
                bounds.width(bounds.width() - width_plus_margin);
                break;
            }
            case dock::top: {
                rect.pos(bounds.left() + margins.left(), bounds.top() + margins.top());
                rect.width(bounds.width() - horizontal_margin);
                bounds.height(bounds.height() - height_plus_margin);
                bounds.top(bounds.top() + height_plus_margin);
                break;
            }
            default:
                break;
        }
    }

    void dock_layout_panel::on_initialize() {
        should_clip(true);
    }

    void dock_layout_panel::on_resize(const core::rect& context_bounds) {
        if (type() != types::container)
            return;

        auto& rect = bounds();
        if (rect.width() == 0 || rect.height() == 0) {
            auto width = 0;
            auto height = 0;
            auto last_left = 0;
            auto last_top = 0;
            for (auto child : children()) {
                child->resize(context_bounds);
                auto child_margins = child->margin();
                auto child_bounds = child->inner_bounds();
                if (child_bounds.left() > last_left) {
                    last_left = child_bounds.left();
                    width = child_bounds.right() + child_margins.horizontal();
                }
                if (child_bounds.top() > last_top) {
                    last_top = child_bounds.top();
                    height = child_bounds.bottom() + child_margins.vertical();
                }
            }
            if (width > rect.width())
                rect.width(width);
            if (height > rect.height())
                rect.height(height);
        }

        core::rect bounds;
        auto parent_view = parent();
        if (parent_view != nullptr) {
            if (parent_view->type() == types::container)
                bounds = inner_bounds();
            else
                bounds = parent_view->inner_bounds();
        } else {
            bounds = context_bounds;
        }

        for (auto child : children()) {
            resize_child(child, bounds);
        }

        if (parent_view == nullptr)
            resize_child(this, bounds);
    }

}