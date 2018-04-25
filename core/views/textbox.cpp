//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <sstream>
#include <core/input_binding.h>
#include <core/input_action.h>
#include "textbox.h"

namespace ryu::core {

    textbox::textbox(
            const std::string& name,
            core::view_host* host) : core::view(types::control, name, host),
                                               _caret("textbox-caret", host) {
    }

    textbox::~textbox() {
    }

    void textbox::clear() {
        _document.clear();
        caret_home();
    }

    void textbox::caret_end() {
        auto line_end = _document.find_line_end();
        for (auto i = 0; i < line_end; i++)
            caret_right();
    }

    void textbox::caret_home() {
        _caret.column(0);
        _document.column(0);
    }

    void textbox::define_actions() {
        auto caret_left_action = core::input_action::create_no_map(
                "textbox_caret_left",
                "Internal",
                "Move the caret left within the textbox.");
        if (!caret_left_action->has_bindings())
            caret_left_action->bind_keys({core::key_left});

        auto caret_right_action = core::input_action::create_no_map(
                "textbox_caret_right",
                "Internal",
                "Move the caret right within the textbox.");
        if (!caret_right_action->has_bindings())
            caret_right_action->bind_keys({core::key_right});

        auto backspace_action = core::input_action::create_no_map(
                "textbox_backspace",
                "Internal",
                "Move the caret left and shift the line left.");
        if (!backspace_action->has_bindings())
            backspace_action->bind_keys({core::key_backspace});

        auto delete_action = core::input_action::create_no_map(
                "textbox_delete",
                "Internal",
                "Shift the line left at the caret position.");
        if (!delete_action->has_bindings())
            delete_action->bind_keys({core::key_delete});

        auto home_action = core::input_action::create_no_map(
                "textbox_home",
                "Internal",
                "Move the caret to the home position.");
        if (!home_action->has_bindings())
            home_action->bind_keys({core::key_home});

        auto end_action = core::input_action::create_no_map(
                "textbox_end",
                "Internal",
                "Move the caret to the end of the line.");
        if (!end_action->has_bindings())
            end_action->bind_keys({core::key_end});

        auto text_input_action = core::input_action::create_no_map(
                "textbox_input",
                "Internal",
                "Any ASCII text input (non-mappable).");
        if (!text_input_action->has_bindings())
            text_input_action->bind_text_input();
    }

    void textbox::bind_events() {
        action_provider().register_handler(
                core::input_action::find_by_name("textbox_caret_left"),
                [this](const core::event_data_t& data) {
                    caret_left();
                    return true;
                });

        action_provider().register_handler(
                core::input_action::find_by_name("textbox_caret_right"),
                [this](const core::event_data_t& data) {
                    caret_right();
                    return true;
                });

        action_provider().register_handler(
                core::input_action::find_by_name("textbox_backspace"),
                [this](const core::event_data_t& data) {
                    if (_caret.column() == 0) {
                        _document.delete_line();
                    } else {
                        caret_left();
                        _document.shift_line_left();
                    }
                    return true;
                });

        action_provider().register_handler(
                core::input_action::find_by_name("textbox_delete"),
                [this](const core::event_data_t& data) {
                    _document.shift_line_left();
                    return true;
                });

        action_provider().register_handler(
                core::input_action::find_by_name("textbox_home"),
                [this](const core::event_data_t& data) {
                    caret_home();
                    return true;
                });

        action_provider().register_handler(
                core::input_action::find_by_name("textbox_end"),
                [this](const core::event_data_t& data) {
                    caret_end();
                    return true;
                });

        action_provider().register_handler(
                core::input_action::find_by_name("textbox_input"),
                [this](const core::event_data_t& data) {
                    if (_on_key_down != nullptr)
                        if (!_on_key_down(data.c))
                            return false;

                    if (data.c == core::ascii_escape
                    ||  data.c == core::ascii_tab
                    ||  data.c == core::ascii_return)
                        return false;

                    _document.put(core::element_t {
                        static_cast<uint8_t>(data.c),
                        _document.default_attr()});
                    caret_right();

                    return true;
                });
    }

    void textbox::on_initialize() {
        tab_stop(true);
        define_actions();
        bind_events();

        _caret.initialize();
        _caret.enabled(false);
        _caret.position(0, 0);
        _caret.page_size(_document.page_height(), _document.page_width());
        add_child(&_caret);

        _document.caret(&_caret);
        _document.document_size(1, 100);
        _document.page_size(1, 16);
        _document.clear();
        _document.on_document_changed([this]() {
            std::stringstream stream;
            _document.write_line(stream, 0, 0, _document.columns());
            view::value(stream.str());
        });
        padding({5, 5, 5, 5});
    }

    uint16_t textbox::width() const {
        return _document.page_width();
    }

    void textbox::on_focus_changed() {
        _caret.enabled(focused());
    }

    uint16_t textbox::length() const {
        return _document.columns();
    }

    void textbox::width(uint8_t value) {
        _document.page_size(1, value);
        _caret.page_size(1, value);
        requires_layout();
    }

    std::string textbox::value() const {
        return view::value();
    }

    void textbox::length(uint16_t value) {
        _document.document_size(1, value);
        requires_layout();
    }

    void textbox::fg_color(uint8_t value) {
        view::fg_color(value);
        _caret.fg_color(value);
        _document.default_attr().color = value;
    }

    void textbox::bg_color(uint8_t value) {
        view::bg_color(value);
        _caret.bg_color(value);
    }

    bool textbox::caret_left(uint8_t columns) {
        auto clamped = _caret.left(columns);
        if (clamped) {
            return _document.scroll_left();
        }
        return clamped;
    }

    bool textbox::caret_right(uint8_t columns) {
        auto clamped = _caret.right(columns);
        if (clamped) {
            return _document.scroll_right();
        }
        return clamped;
    }

    void textbox::value(const std::string& value) {
        std::stringstream stream(value);
        core::result result;
        if (!_document.load(result, stream)) {
            _document.clear();
        }
    }

    void textbox::on_draw(core::renderer& surface) {
        auto bounds = client_bounds();

        auto pal = *view::palette();
        auto fg = pal[view::fg_color()];
        auto& bg = pal[view::bg_color()];

        if (!enabled() || !focused()) {
            fg = fg - 35;
        }

        surface.set_color(bg);
        surface.fill_rect(bounds);

        surface.set_color(fg);
        surface.set_font_color(font_face(), fg);

        surface.draw_text_aligned(
            font_face(),
            value(),
            bounds,
            alignment::horizontal::left,
            alignment::vertical::middle);
        surface.draw_line(
            bounds.left(),
            bounds.bottom(),
            bounds.right() + 5,
            bounds.bottom());
    }

    void textbox::palette(core::palette* value) {
        view::palette(value);
        _caret.palette(value);
    }

    void textbox::font_family(core::font_family* value) {
        view::font_family(value);
        _caret.font_family(value);
    }

    void textbox::on_resize(const core::rect& context_bounds) {
        switch (sizing()) {
            case sizing::content:
            case sizing::fixed: {
                bounds().size(
                    font_face()->width * (_document.page_width() + 1),
                    font_face()->line_height + 10);
                break;
            }
            case sizing::parent: {
                auto container = parent();
                auto rect = container != nullptr ? container->bounds() : context_bounds;
                auto& margins = margin();
                bounds().size(
                    rect.width() - margins.right(),
                    font_face()->line_height + 10);
                break;
            }
        }
    }

    void textbox::on_key_down(const textbox::on_key_down_callable& callable) {
        _on_key_down = callable;
    }

}