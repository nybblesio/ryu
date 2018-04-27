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

#include <utility>
#include <core/view.h>
#include <core/document.h>
#include <core/input_action.h>
#include <core/text_formatter.h>
#include "caret.h"

namespace ryu::core {

    class console;

    struct output_process_result_t {
        uint32_t line_count;
        bool more_to_process;
    };

    struct output_queue_entry_t {
        size_t msg_index = 0;
        size_t line_index = 0;
        core::result result;
        core::formatted_text_list lines;

        explicit output_queue_entry_t(core::result value) : result(std::move(value)) {
        }

        output_process_result_t process(console* c);
    };

    class console : public core::view {
    public:
        using caret_changed_callable = std::function<void(const core::caret&, const core::document&)>;
        using execute_command_callable = std::function<bool(core::result&, const std::string&)>;
        using command_action_dict = std::map<core::system_commands::types, std::function<bool (core::console&, const parameter_dict&)>>;

        enum states {
            input,
            pre_processing,
            resume_processing,
            processing,
            post_processing,
            wait,
        };

        console(
            const std::string& name,
            core::view_host* host);

        ~console() override;

        void page_up();

        void page_down();

        void caret_end();

        void last_page();

        void first_page();

        void caret_home();

        void caret_line_end();

        uint32_t write_message(
                const std::string& message,
                bool last_newline = true);

        uint32_t write_message(
                const formatted_text_t& formatted_text,
                bool last_newline = true);

        bool more() const;

        void caret_up_line();

        void more(bool flag);

        void caret_newline();

        void caret_color(uint8_t color);

        void caret_up(uint8_t rows = 1);

        void caret_down(uint8_t rows = 1);

        void selection_color(uint8_t value);

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

        void on_initialize() override;

        void on_draw(core::renderer& surface) override;

        void on_resize(const core::rect& context_bounds) override;

        void on_update(uint32_t dt, core::pending_event_list& events) override;

    private:
        friend struct output_queue_entry_t;

        void bind_events();

        void define_actions();

        bool transition_to(
                core::state_transition_command command,
                const core::parameter_dict& params);

        void format_data_table(
                formatted_text_list& list,
                data_table_t& table);

        void format_command_result(
                const parameter_variant_t& param,
                formatted_text_list& lines);

        std::string get_alignment_format(
                core::alignment::horizontal::types value);

        void on_process_command();

        void raise_caret_changed();

        void on_pre_process_command();

        void calculate_page_metrics();

        void on_post_process_command();

        void on_resume_process_command();

        std::string find_command_string();

        void select_color(const std::string& name);

        void scale_columns(std::vector<data_table_column_t>& columns);

    private:
        static command_action_dict s_handlers;

        bool _more {};
        caret _caret;
        uint8_t _color;
        metrics_t _metrics;
        document _document;
        uint8_t _selection_color;
        int16_t _remaining_lines = 0;
        states _state = states::input;
        code_to_attr_dict _code_mapper;
        std::deque<output_queue_entry_t> _output_queue;
        caret_changed_callable _caret_changed_callback;
        state_transition_callable _transition_to_callback;
        execute_command_callable _execute_command_callback;
    };

};

