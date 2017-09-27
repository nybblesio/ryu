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
        using on_key_down_callable = std::function<void (int)>;

        textbox(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        void clear();

        std::string value();

        void size(int rows, int columns);

        void value(const std::string& value);

        void initialize(int rows, int columns);

        void on_key_down(const on_key_down_callable& callable);

    protected:
        void caret_end();

        void caret_home();

        void on_draw() override;

        void on_resize() override;

        void on_focus_changed() override;

        bool caret_left(int columns = 1);

        bool caret_right(int columns = 1);

        bool on_process_event(const SDL_Event* e) override;

    private:
        int _page_width;
        core::caret _caret;
        core::document _document;
        on_key_down_callable _on_key_down;
    };

};

