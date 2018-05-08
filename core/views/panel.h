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

#include <core/view.h>

namespace ryu::core {

    class panel : public core::view {
    public:
        enum class layout_modes : uint16_t {
            flow,
            flex
        };

        enum class flex_directions : uint16_t {
            none,
            row,
            column
        };

        enum class layout_justifications : uint16_t {
            start,
            middle,
            end,
            full
        };

        panel(
            const std::string& name,
            core::view_host* host);

        ~panel() override;

        bool layout_wrap() const;

        void layout_wrap(bool value);

        panel::layout_modes layout_mode() const;

        void layout_mode(panel::layout_modes value);

        panel::flex_directions flex_direction() const;

        void flex_direction(panel::flex_directions value);

        panel::layout_justifications layout_justification() const;

        void layout_justification(panel::layout_justifications value);

    protected:
        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

    private:
        bool _layout_wrap = false;
        layout_modes _mode = layout_modes::flow;
        flex_directions _direction = flex_directions::none;
        layout_justifications _justification = layout_justifications::start;
    };

};