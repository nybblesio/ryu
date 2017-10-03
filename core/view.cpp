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
#include "id_pool.h"

namespace ryu::core {

    view::view(
            types::id type,
            const std::string& name) : _id(core::id_pool::instance()->allocate()),
                                       _name(name),
                                       _type(type) {
    }

    view::~view() {
        core::id_pool::instance()->release(_id);
    }

    int view::id() const {
        return _id;
    }

    void view::focus(int id) {
        for (auto child : _children)
            child->focus(id);
        focus(id == _id);
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
        return (_flags & config::flags::visible) != 0;
    }

    bool view::tabstop() const {
        return (_flags & config::flags::tabstop) != 0;
    }

    bool view::focused() const {
        return (_flags & config::flags::focused) != 0;
    }

    void view::clear_children() {
        for (auto child : _children)
            child->_parent = nullptr;
        _children.clear();
    }

    void view::focus(bool value) {
        if (value)
            _flags |= config::flags::focused;
        else
            _flags &= ~config::flags::focused;
        if (_in_on_focus_changed)
            return;
        _in_on_focus_changed = true;
        on_focus_changed();
        _in_on_focus_changed = false;
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

    core::rect view::client_bounds() {
        auto rect = bounds();
        auto pad = padding();
        core::rect padded;
        padded.pos(rect.left() + pad.left(), rect.top() + pad.top());
        padded.size(rect.width() - pad.right(), rect.height() - pad.bottom());
        return padded;
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

    void view::bg_color(uint8_t value) {
        _bg_color = value;
    }

    void view::fg_color(uint8_t value) {
        _fg_color = value;
    }

    view::types::id view::type() const {
        return _type;
    }

    core::dock::styles view::dock() const {
        return _dock;
    }

    void view::font_style(uint8_t styles) {
        _font_style = styles;
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

        on_draw(renderer);
        for (auto child : _children)
            child->draw(renderer);
    }

    void view::remove_child(core::view* child) {
        if (child == nullptr)
            return;
        _children.erase(std::remove(_children.begin(), _children.end(), child));
    }

    void view::bounds(const core::rect& value) {
        _rect = value;
    }

    void view::palette(core::palette* palette) {
        _palette = palette;
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

    bool view::process_event(const SDL_Event* e) {
        if (focused()) {
            if (e->type == SDL_KEYDOWN) {
                switch (e->key.keysym.sym) {
                    case SDLK_TAB: {
                        if (_on_tab_callable != nullptr) {
                            _on_tab_callable();
                            return true;
                        }
                        break;
                    }
                }
            }
            if (on_process_event(e))
                return true;
        }

        for (auto child : _children) {
            if (child->process_event(e))
                return true;
        }

        return false;
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

    bool view::on_process_event(const SDL_Event* e) {
        return false;
    }

    void view::font_family(core::font_family* font) {
        _font = font;
    }

    void view::resize(const core::rect& context_bounds) {
        on_resize(context_bounds);
        for (auto child : _children)
            child->resize(context_bounds);
    }

    void view::on_resize(const core::rect& context_bounds) {
    }

    void view::on_tab(const view::on_tab_callable& callable) {
        _on_tab_callable = callable;
    }

}