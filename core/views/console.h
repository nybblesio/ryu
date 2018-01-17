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

    using code_to_attr_callable = std::function<void (attr_t&)>;
    typedef std::map<std::string, code_to_attr_callable> code_to_attr_dict;

    class console : public core::view {
    public:
        using caret_changed_callable = std::function<void (const core::caret&)>;
        using execute_command_callable = std::function<bool (core::result&, const std::string&)>;

        enum states {
            input,
            processing,
            wait,
        };

        explicit console(const std::string& name);

        void caret_end();

        void caret_home();

        void initialize();

        uint32_t write_message(
                const std::string& message,
                bool last_newline = true);

        void update(uint32_t dt);

        void caret_color(uint8_t color);

        void caret_up(uint8_t rows = 1);

        void caret_down(uint8_t rows = 1);

        bool caret_left(uint8_t columns = 1);

        bool caret_right(uint8_t columns = 1);

        void code_mapper(const code_to_attr_dict& value);

        void on_caret_changed(const caret_changed_callable& callable);

        void on_execute_command(const execute_command_callable& callable);

        void on_transition(const core::state_transition_callable& callable);

    protected:
        struct metrics_t {
            uint8_t page_width = 1;
            uint8_t page_height = 1;
            const int left_padding = 10;
            const int right_padding = 10;
        };

        void on_draw(core::renderer& surface) override;

        bool on_process_event(const SDL_Event* e) override;

        void on_resize(const core::rect& context_bounds) override;

    private:
        void raise_caret_changed();

        void calculate_page_metrics();

        std::string find_command_string();

        void process_command_result_queue();

        bool transition_to(const std::string& name, const core::parameter_dict& params);

    private:
        caret _caret;
        uint8_t _color;
        metrics_t _metrics;
        document _document;
        int16_t _remaining_lines = 0;
        states _state = states::input;
        code_to_attr_dict _code_mapper;
        size_t _current_result_message_index = 0;
        std::deque<core::result> _command_result_queue;
        caret_changed_callable _caret_changed_callback;
        state_transition_callable _transition_to_callback;
        execute_command_callable _execute_command_callback;
    };

};

