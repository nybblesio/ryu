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

        void size(int rows, int columns);

        void value(const std::string& value);

        void initialize(int rows, int columns);

        void on_key_down(const on_key_down_callable& callable);

    protected:
        void caret_end();

        void caret_home();

        void on_focus_changed() override;

        bool caret_left(int columns = 1);

        bool caret_right(int columns = 1);

        void on_draw(SDL_Renderer* renderer) override;

        bool on_process_event(const SDL_Event* e) override;

        void on_resize(const core::rect& context_bounds) override;

    private:
        core::caret _caret;
        int _page_width = 128;
        core::document _document;
        on_key_down_callable _on_key_down {};
    };

};

