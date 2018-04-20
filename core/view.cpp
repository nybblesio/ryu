//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <utility>
#include "view.h"
#include "input_action.h"

namespace ryu::core {

    view::view(
            types::id type,
            const std::string& name,
            core::view_host* host) : _id(core::id_pool::instance()->allocate()),
                                     _name(name),
                                     _type(type),
                                     _host(host) {
    }

    view::~view() {
        core::id_pool::instance()->release(_id);
    }

    view_host* view::host() {
        return _host;
    }

    void view::bind_events() {
//        field_shift_tab_action->register_handler(
//                action_sink::types::view,
//                [this](const core::event_data_t& data) {
//                    return focused() && visible();
//                },
//                [this](const core::event_data_t& data) {
//                    if (_prev != nullptr) {
//                        find_root()->focus(_prev);
//                        return true;
//                    }
//                    return false;
//                });

//        field_tab_action->register_handler(
//            action_sink::types::view,
//            [this](const core::event_data_t& data) {
//                return focused() && visible();
//            },
//            [this](const core::event_data_t& data) {
//                if (_next != nullptr) {
//                    find_root()->focus(_next);
//                    return true;
//                }
//                return false;
//            });
    }

    void view::define_actions() {
        auto field_shift_tab_action = core::input_action::create_no_map(
                "view_field_shift_tab",
                "Internal",
                "Move focus from the current field to the previous field.");
        field_shift_tab_action->bind_keys({core::mod_shift, core::key_tab});

        auto field_tab_action = core::input_action::create_no_map(
                "view_field_tab",
                "Internal",
                "Move focus from the current field to the next field.");
        field_tab_action->bind_keys({core::key_tab});
    }

    void view::initialize() {
        listen_for_on_host_change();
        on_initialize();
        define_actions();
    }

    uint32_t view::id() const {
        return _id;
    }

    bool view::layout() const {
        return (_flags & config::flags::layout) != 0;
    }

    short view::index() const {
        return _index;
    }

    core::rect& view::bounds() {
        return _rect;
    }

    core::view* view::parent() {
        return _parent;
    }

    bool view::enabled() const {
        return (_flags & config::flags::enabled) != 0;
    }

    bool view::visible() const {
        return _host->is_visible()
               && (_flags & config::flags::visible) != 0;
    }

    bool view::focused() const {
        auto host_focus = _host->is_focused();
        auto flag = (_flags & config::flags::focused) != 0;
        return host_focus && flag;
    }

    void view::on_initialize() {
    }

    bool view::tab_stop() const {
        return (_flags & config::flags::tab_stop) != 0;
    }

    void view::clear_children() {
        for (auto child : _children)
            child->_parent = nullptr;
        _children.clear();
    }

    view_list& view::children() {
        return _children;
    }

    void view::requires_layout() {
        auto container = parent();
        if (container != nullptr)
            container->layout(true);
        else
            layout(true);
    }

    void view::layout(bool value) {
        if (value)
            _flags |= config::flags::layout;
        else
            _flags &= ~config::flags::layout;
    }

    core::padding& view::margin() {
        return _margin;
    }

    void view::index(short value) {
        _index = value;
    }

    void view::on_focus_changed() {
    }

    core::view* view::find_root() {
        auto current = this;
        while (true) {
            auto next = current->parent();
            if (next == nullptr)
                break;
            current = next;
        }
        return current;
    }

    core::palette* view::palette() {
        return _palette;
    }

    void view::enabled(bool value) {
        if (value)
            _flags |= config::flags::enabled;
        else
            _flags &= ~config::flags::enabled;
    }

    core::padding& view::padding() {
        return _padding;
    }

    void view::visible(bool value) {
        if (value)
            _flags |= config::flags::visible;
        else
            _flags &= ~config::flags::visible;

        for (auto child : _children)
            child->visible(value);
    }

    void view::tab_stop(bool value) {
        if (value)
            _flags |= config::flags::tab_stop;
        else
            _flags &= ~config::flags::tab_stop;
    }

    std::string view::name() const {
        return _name;
    }

    uint8_t view::bg_color() const {
        return _bg_color;
    }

    uint8_t view::fg_color() const {
        return _fg_color;
    }

    uint8_t view::font_style() const {
        return _font_style;
    }

    core::rect view::client_bounds() {
        auto rect = bounds();
        auto pad = padding();
        core::rect padded;
        padded.pos(
                rect.left() + pad.left(),
                rect.top() + pad.top());
        padded.size(
                std::max(0, rect.width() - pad.right()),
                std::max(0, rect.height() - pad.bottom()));
        return padded;
    }

    void view::bg_color(uint8_t value) {
        _bg_color = value;
    }

    void view::fg_color(uint8_t value) {
        _fg_color = value;
    }

    view::types::id view::type() const {
        return _type;
    }

    void view::inner_focus(bool value) {
        if (value)
            _flags |= config::flags::focused;
        else
            _flags &= ~config::flags::focused;
        on_focus_changed();
    }

    core::dock::styles view::dock() const {
        return _dock;
    }

    void view::font_style(uint8_t styles) {
        _font_style = styles;
    }

    void view::next_view(core::view* value) {
        _next = value;
    }

    void view::prev_view(core::view* value) {
        _prev = value;
    }

    void view::add_child(core::view* child) {
        if (child == nullptr)
            return;
        child->_parent = this;
        _children.push_back(child);
    }

    view::sizing::types view::sizing() const {
        return _sizing;
    }

    void view::dock(core::dock::styles style) {
        _dock = style;
    }

    void view::draw(core::renderer& renderer) {
        if (!visible())
            return;

        if (layout()) {
            resize(renderer.bounds());
        }

        on_draw(renderer);
        draw_children(renderer);
    }

    void view::remove_child(core::view* child) {
        if (child == nullptr)
            return;
        _children.erase(std::find(_children.begin(), _children.end(), child));
    }

    void view::bounds(const core::rect& value) {
        _rect = value;
    }

    void view::palette(core::palette* value) {
        _palette = value;
    }

    const core::font_t* view::font_face() const {
        auto family = font_family();
        if (family == nullptr)
            return nullptr;
        return family->find_style(_font_style);
    }

    void view::sizing(view::sizing::types value) {
        _sizing = value;
    }

    void view::on_draw(core::renderer& renderer) {
    }

    void view::focus(const core::view* target) {
        if (target == nullptr)
            return;

        for (auto child : _children)
            child->focus(target);

        inner_focus(target->_id == this->_id);
    }

    void view::listen_for_on_host_change() {
        _host->on_change(std::bind(&view::on_host_changed, this, std::placeholders::_1));
    }

    core::view* view::get_child_at(size_t index) {
        if (index < _children.size())
            return _children[index];
        return nullptr;
    }

    core::font_family* view::font_family() const {
        return _font;
    }

    void view::margin(const core::padding& value) {
        _margin = value;
    }

    void view::padding(const core::padding& value) {
        _padding = value;
    }

    void view::font_family(core::font_family* value) {
        _font = value;
    }

    void view::draw_children(core::renderer& renderer) {
        for (auto child : _children)
            child->draw(renderer);
    }

    void view::resize(const core::rect& context_bounds) {
        if (!visible())
            return;

        layout(false);
        on_resize(context_bounds);
        for (auto child : _children)
            child->resize(context_bounds);
    }

    void view::on_resize(const core::rect& context_bounds) {
    }

    void view::on_host_changed(view_host::change_reason_flags flags) {
        on_focus_changed();
    }

}