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
#include <core/document.h>
#include "caret.h"

namespace ryu::core {

    class textbox : public core::view {
    public:
        using on_key_down_callable = std::function<bool (int)>;

        explicit textbox(const std::string& name);

        void clear();

        std::string value();

        int length() const {
            return _document.columns();
        }

        void value(const std::string& value);

        void size(uint8_t rows, uint8_t columns);

        void initialize(uint8_t rows, uint8_t columns);

        void on_key_down(const on_key_down_callable& callable);

    protected:
        void caret_end();

        void caret_home();

        void on_focus_changed() override;

        bool caret_left(uint8_t columns = 1);

        bool caret_right(uint8_t columns = 1);

        void on_draw(core::renderer& surface) override;

        bool on_process_event(const SDL_Event* e) override;

        void on_resize(const core::rect& context_bounds) override;

    private:
        core::caret _caret;
        core::document _document;
        uint8_t _page_width = 128;
        on_key_down_callable _on_key_down {};
    };

};

