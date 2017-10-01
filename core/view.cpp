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
            core::context* context,
            types::id type,
            const std::string& name) : _id(core::id_pool::instance()->allocate()),
                                _name(name),
                                _type(type),
                                _context(context) {
        visible(true);
    }

    view::~view() {
        core::id_pool::instance()->release(_id);
    }

    void view::draw() {
        if (!visible())
            return;

        on_draw();
        for (auto child : _children)
            child->draw();
    }

    void view::resize() {
        on_resize();
        for (auto child : _children)
            child->resize();
    }

    int view::id() const {
        return _id;
    }

    void view::on_draw() {
    }

    core::view* view::parent() {
        return _parent;
    }

    void view::on_resize() {
        if (type() != types::container)
            return;

        core::rect bounds;
        auto parent_view = parent();
        if (parent_view != nullptr) {
            if (parent_view->type() == types::container)
                bounds = this->bounds();
            else
                bounds = parent_view->bounds();
        } else {
            bounds = context()->bounds();
        }

        view_list views = _children;
        if (parent_view == nullptr)
            views.push_back(this);

        for (auto child : views) {
            auto& rect = child->bounds();
            auto margins = child->margin();
            auto horizontal_margin = margins.left() + margins.right();
            auto width_plus_margin = rect.width() + horizontal_margin;
            auto vertical_margin = margins.top() + margins.bottom();
            auto height_plus_margin = rect.height() + vertical_margin;
            switch (child->dock()) {
                case dock::bottom: {
                    rect.pos(bounds.left() + margins.left(), bounds.bottom() - height_plus_margin);
                    rect.width(bounds.width() - margins.right());
                    bounds.height(bounds.height() - height_plus_margin);
                    break;
                }
                case dock::fill: {
                    rect = {bounds.left() + margins.left(),
                            bounds.top() + margins.top(),
                            bounds.width() - margins.right(),
                            bounds.height() - margins.bottom()};
                    break;
                }
                case dock::left: {
                    rect.pos(bounds.left() + margins.left(), bounds.top() + margins.top());
                    rect.height(bounds.height() - vertical_margin);
                    bounds.width(bounds.width() - width_plus_margin);
                    bounds.left(bounds.left() + width_plus_margin);
                    break;
                }
                case dock::right: {
                    rect.pos(bounds.right() - width_plus_margin, bounds.top() + margins.top());
                    rect.height(bounds.height() - vertical_margin);
                    bounds.width(bounds.width() - width_plus_margin);
                    break;
                }
                case dock::top: {
                    rect.pos(bounds.left() + margins.left(), bounds.top() + margins.top());
                    rect.width(bounds.width() - horizontal_margin);
                    bounds.height(bounds.height() - height_plus_margin);
                    bounds.top(bounds.top() + height_plus_margin);
                    break;
                }
                default:
                    continue;
            }
        }
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

    void view::pop_blend_mode() {
        if (_mode_stack.empty())
            return;
        auto mode = _mode_stack.top();
        _mode_stack.pop();
        SDL_SetRenderDrawBlendMode(context()->renderer(), mode);
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

    core::context* view::context() {
        return _context;
    }

    core::palette* view::palette() {
        if (_palette == nullptr)
            return _context->palette();
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

    core::font_family* view::font_family() const {
        if (_font == nullptr) {
            return _context->engine()->find_font_family("topaz");
        }
        return _font;
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

    view::dock::styles view::dock() const {
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

    void view::remove_child(core::view* child) {
        if (child == nullptr)
            return;
        _children.erase(std::remove(_children.begin(), _children.end(), child));
    }

    void view::bounds(const core::rect& value) {
        _rect = value;
    }

    void view::dock(view::dock::styles style) {
        _dock = style;
    }

    void view::palette(core::palette* palette) {
        _palette = palette;
    }

    bool view::process_event(const SDL_Event* e) {
        if (focused()) {
            if (e->type == SDL_KEYDOWN) {
                switch (e->key.keysym.sym) {
                    case SDLK_TAB: {
                        if (_on_tab_callable != nullptr)
                            _on_tab_callable();
                        return true;
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

    void view::margin(const core::padding& value) {
        _margin = value;
    }

    void view::padding(const core::padding& value) {
        _padding = value;
    }

    void view::draw_rect(const core::rect& bounds) {
        auto rect = bounds.to_sdl_rect();
        SDL_RenderDrawRect(context()->renderer(), &rect);
    }

    void view::fill_rect(const core::rect& bounds) {
        auto rect = bounds.to_sdl_rect();
        SDL_RenderFillRect(context()->renderer(), &rect);
    }

    void view::push_blend_mode(SDL_BlendMode mode) {
        SDL_BlendMode previous_blend_mode;
        SDL_GetRenderDrawBlendMode(context()->renderer(), &previous_blend_mode);
        _mode_stack.push(previous_blend_mode);
        SDL_SetRenderDrawBlendMode(context()->renderer(), mode);
    }

    bool view::on_process_event(const SDL_Event* e) {
        return false;
    }

    void view::font_family(core::font_family* font) {
        _font = font;
    }

    int view::measure_text(const std::string& value) {
        return FC_GetWidth(font_face()->glyph, value.c_str());
    }

    void view::draw_line(int x1, int y1, int x2, int y2) {
        SDL_RenderDrawLine(context()->renderer(), x1, y1, x2, y2);
    }

    void view::set_color(const core::palette_entry& color) {
        SDL_SetRenderDrawColor(
                context()->renderer(),
                color.red(),
                color.green(),
                color.blue(),
                color.alpha());
    }

    void view::on_tab(const view::on_tab_callable& callable) {
        _on_tab_callable = callable;
    }

    void view::set_font_color(const core::palette_entry& color) {
        FC_SetDefaultColor(font_face()->glyph, color.to_sdl_color());
    }

    void view::draw_text(
            int x,
            int y,
            const std::string& value,
            const core::palette_entry& color) {
        FC_DrawColor(
                font_face()->glyph,
                context()->renderer(),
                x,
                y,
                color.to_sdl_color(),
                value.c_str());
    }

    void view::draw_text_aligned(
            const std::string& value,
            const core::rect& bounds,
            alignment::horizontal::types halign,
            alignment::vertical::types valign) {
        auto yoffset = 0;
        switch (valign) {
            case alignment::vertical::bottom:
                yoffset = bounds.height() - font_face()->line_height;
                break;
            case alignment::vertical::middle:
                yoffset = (bounds.height() - font_face()->line_height) / 2;
                break;
            default:
                break;
        }
        auto box = SDL_Rect{bounds.left(), bounds.top() + yoffset, bounds.width(), bounds.height()};
        FC_DrawBoxAlign(
                font_face()->glyph,
                context()->renderer(),
                box,
                alignment::to_font_align(halign),
                value.c_str());
    }

}