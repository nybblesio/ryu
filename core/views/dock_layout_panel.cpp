//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "dock_layout_panel.h"

namespace ryu::core {

    dock_layout_panel::dock_layout_panel(const std::string& name) : panel(name) {
    }

    void dock_layout_panel::on_resize(const core::rect& context_bounds) {
        if (type() != types::container)
            return;

        core::rect bounds;
        auto parent_view = parent();
        if (parent_view != nullptr) {
            if (parent_view->type() == types::container)
                bounds = client_bounds();
            else
                bounds = parent_view->client_bounds();
        } else {
            bounds = context_bounds;
        }

        view_list views;
        for (auto child : children())
            views.push_back(child);
        if (parent_view == nullptr)
            views.push_back(this);

        for (auto child : views) {
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
                    rect = {bounds.left() + margins.left(),
                            bounds.top() + margins.top(),
                            bounds.width() - margins.right(),
                            bounds.height() - margins.bottom()};
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
                    continue;
            }
        }
    }

}