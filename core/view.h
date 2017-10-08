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

#include <string>
#include <vector>
#include <cstdint>
#include <SDL_events.h>
#include <SDL_render.h>
#include "rect.h"
#include "palette.h"
#include "context.h"
#include "padding.h"
#include "renderer.h"
#include "font_family.h"

namespace ryu::core {

    class view {
    public:
        using on_tab_callable = std::function<void ()>;

        struct sizing {
            enum types {
                content,
                fixed,
                parent
            };
        };

        struct config {
            enum flags : uint8_t {
                none    = 0b00000000,
                visible = 0b00000001,
                enabled = 0b00000010,
                tabstop = 0b00000100,
                focused = 0b00001000,
                layout  = 0b00010000
            };
        };

        struct types {
            enum id : uint8_t {
                control,
                container
            };
        };

        view(types::id type, const std::string& name);

        virtual ~view();

        int id() const;

        view* parent();

        void focus(int id);

        bool layout() const;

        short index() const;

        core::rect& bounds();

        bool focused() const;

        bool enabled() const;

        bool visible() const;

        bool tabstop() const;

        void clear_children();

        view_list& children();

        void requires_layout();

        types::id type() const;

        void layout(bool value);

        core::padding& margin();

        void index(short value);

        core::padding& padding();

        void enabled(bool value);

        void visible(bool value);

        void tabstop(bool value);

        std::string name() const;

        core::palette* palette();

        uint8_t bg_color() const;

        uint8_t fg_color() const;

        dock::styles dock() const;

        uint8_t font_style() const;

        core::rect client_bounds();

        void dock(dock::styles style);

        void font_style(uint8_t styles);

        void add_child(core::view* child);

        view::sizing::types sizing() const;

        void draw(core::renderer& renderer);

        void remove_child(core::view* child);

        virtual void bg_color(uint8_t value);

        virtual void fg_color(uint8_t value);

        void bounds(const core::rect& value);

        const core::font_t* font_face() const;

        void sizing(view::sizing::types value);

        bool process_event(const SDL_Event* e);

        core::font_family* font_family() const;

        void margin(const core::padding& value);

        void padding(const core::padding& value);

        void on_tab(const on_tab_callable& callable);

        virtual void palette(core::palette* palette);

        void resize(const core::rect& context_bounds);

        virtual void font_family(core::font_family* font);

    protected:
        view* find_root();

        void focus(bool value);

        virtual void on_focus_changed();

        virtual void on_draw(core::renderer& renderer);

        virtual bool on_process_event(const SDL_Event* e);

        virtual void on_resize(const core::rect& context_bounds);

    private:
        int _id;
        short _index = 0;
        std::string _name;
        core::rect _rect {};
        uint8_t _bg_color = 0;
        uint8_t _fg_color = 0;
        view_list _children {};
        core::padding _margin {};
        core::padding _padding {};
        core::view* _parent = nullptr;
        types::id _type = types::control;
        on_tab_callable _on_tab_callable;
        bool _in_on_focus_changed = false;
        core::palette* _palette = nullptr;
        core::font_family* _font = nullptr;
        uint8_t _font_style = font::styles::normal;
        core::dock::styles _dock = dock::styles::none;
        view::sizing::types _sizing = view::sizing::types::content;
        uint8_t _flags = config::flags::enabled | config::flags::visible;
    };

};

