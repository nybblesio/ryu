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
#include "engine.h"

namespace ryu::core {

    view::view(
            core::context* context,
            core::view* parent,
            types::id type,
            int id,
            std::string name) : _id(id),
                                _name(std::move(name)),
                                _parent(parent),
                                _type(type),
                                _context(context) {
        if (_parent != nullptr)
            _parent->_children.push_back(this);

        enabled(true);
        visible(true);
    }

    int view::id() const {
        return _id;
    }

    view* view::parent() {
        return _parent;
    }

    core::rect& view::rect() {
        return _rect;
    }

    void view::focus(int id) {
        focus(id == _id);
        for (auto child : _children)
            child->focus(id);
    }

    short view::index() const {
        return _index;
    }

    bool view::enabled() const {
        return (_flags & config::flags::enabled) != 0;
    }

    bool view::visible() const {
        return (_flags & config::flags::visible) != 0;
    }

    bool view::tabstop() const {
        return (_flags & config::flags::tabstop) != 0;
    }

    bool view::focused() const {
        return (_flags & config::flags::focused) != 0;
    }

    void view::focus(bool value) {
        if (value)
            _flags |= config::flags::focused;
        else
            _flags &= ~config::flags::focused;
        on_focus_changed();
    }

    view_list& view::children() {
        return _children;
    }

    core::padding& view::margin() {
        return _margin;
    }

    void view::index(short value) {
        _index = value;
    }

    core::context* view::context() {
        return _context;
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
    }

    void view::tabstop(bool value) {
        if (value)
            _flags |= config::flags::tabstop;
        else
            _flags &= ~config::flags::tabstop;
    }

    void view::on_focus_changed() {
    }

    std::string view::name() const {
        return _name;
    }

    core::rect view::client_rect() {
        auto& bounds = rect();
        auto& pad = padding();
        core::rect padded;
        padded.pos(bounds.left() + pad.left(), bounds.top() + pad.top());
        padded.size(bounds.width() - pad.right(), bounds.height() - pad.bottom());
        return padded;
    }

    uint8_t view::bg_color() const {
        return _bg_color;
    }

    uint8_t view::fg_color() const {
        return _fg_color;
    }

    core::font_t* view::font() const {
        if (_font == nullptr)
            return _context->engine()->find_font("topaz-8");
        return _font;
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

    void view::font(core::font_t* font) {
        _font = font;
    }

    view::dock::styles view::dock() const {
        return _dock;
    }

    void view::draw(SDL_Renderer* renderer) {
        if (!visible())
            return;

        on_draw(renderer);
        for (auto child : _children)
            child->draw(renderer);
    }

    void view::rect(const core::rect& value) {
        _rect = value;
    }

    void view::dock(view::dock::styles style) {
        _dock = style;
    }

    void view::palette(core::palette* palette) {
        _palette = palette;
    }

    void view::on_draw(SDL_Renderer* renderer) {
    }

    bool view::process_event(const SDL_Event* e) {
        if (focused() && on_process_event(e))
            return true;

        for (auto child : _children) {
            if (child->process_event(e))
                return true;
        }

        return false;
    }

    void view::margin(const core::padding& value) {
        _margin = value;
    }

    void view::padding(const core::padding& value) {
        _padding = value;
    }

    bool view::on_process_event(const SDL_Event* e) {
        return false;
    }

}