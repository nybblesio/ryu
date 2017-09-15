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
#include <core/selection.h>
#include <core/views/caret.h>
#include <core/views/textbox.h>

namespace ryu::ide::text_editor {

    namespace fs = boost::filesystem;

    class editor_view : public core::view {
    public:
        using transition_to_callable = std::function<bool (const std::string&)>;
        using execute_command_callable = std::function<void (core::result&, const std::string&)>;

        enum ids {
            caret = 2,
            header_label,
            footer_label,
            command_line
        };

        editor_view(
                core::context* context,
                core::view* parent,
                int id,
                const std::string& name);

        ~editor_view() override;

        void clear();

        void goto_line(int row);

        void load(const fs::path& path);

        void save(const fs::path& path);

        void find(const std::string& needle);

        void initialize(int rows, int columns);

        void on_transition(const transition_to_callable& callable);

        void on_execute_command(const execute_command_callable& callable);

    protected:
        void on_draw() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        void page_up();

        void last_page();

        void page_down();

        void caret_end();

        void scroll_up();

        void caret_home();

        void first_page();

        void scroll_left();

        void scroll_down();

        bool scroll_right();

        void end_selection();

        void update_selection();

        void caret_up(int rows = 1);

        void caret_down(int rows = 1);

        void update_virtual_position();

        void on_focus_changed() override;

        void caret_left(int columns = 1);

        bool caret_right(int columns = 1);

    private:
        int _vcol;
        int _vrow;
        int _page_width;
        int _page_height;
        core::caret _caret;
        core::label _header;
        core::label _footer;
        core::document _document;
        core::selection _selection;
        core::textbox _command_line;
        transition_to_callable _transition_callable;
        execute_command_callable _execute_command_callable;
    };

};

