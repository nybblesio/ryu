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
#include <core/context.h>
#include <core/document.h>
#include <core/views/caret.h>
#include <core/views/label.h>

namespace ryu::ide::console {

    class view : public core::view {
    public:
        using execute_command_callable = std::function<bool (core::result&, const std::string&)>;

        enum ids {
            caret = 2,
            header_label,
            footer_label,
        };

        view(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        ~view() override;

        void caret_end();

        void caret_home();

        void initialize();

        void caret_up(int rows = 1);

        void caret_down(int rows = 1);

        bool caret_left(int columns = 1);

        bool caret_right(int columns = 1);

        void write_message(const std::string& message);

        void on_execute_command(const execute_command_callable& callable);

        void on_transition(const core::state_transition_callable& callable);

    protected:
        void on_draw() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        bool transition_to(const std::string& name, const core::parameter_dict& params);

    private:
        uint8_t _color;
        short _page_width;
        short _page_height;
        core::caret _caret;
        core::label _header;
        core::label _footer;
        core::document _document;
        execute_command_callable _execute_command_callback;
        core::state_transition_callable _transition_to_callback;
    };

};

