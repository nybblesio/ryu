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

        struct config {
            enum flags : uint8_t {
                none    = 0b00000000,
                visible = 0b00000001,
                enabled = 0b00000010,
                tabstop = 0b00000100,
                focused = 0b00001000,
            };
        };

        struct dock {
            enum styles : uint8_t {
                none,
                bottom,
                fill,
                left,
                right,
                top
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

        short index() const;

        core::rect& bounds();

        bool focused() const;

        bool enabled() const;

        bool visible() const;

        bool tabstop() const;

        void clear_children();

        view_list& children();

        types::id type() const;

        core::padding& margin();

        core::padding& padding();

        void index(short value);

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

        void bg_color(uint8_t value);

        void fg_color(uint8_t value);

        void dock(dock::styles style);

        void font_style(uint8_t styles);

        void add_child(core::view* child);

        void draw(core::renderer& renderer);

        void remove_child(core::view* child);

        void bounds(const core::rect& value);

        void palette(core::palette* palette);

        bool process_event(const SDL_Event* e);

        core::font_family* font_family() const;

        void margin(const core::padding& value);

        void padding(const core::padding& value);

        void font_family(core::font_family* font);

        void on_tab(const on_tab_callable& callable);

        void resize(const core::rect& context_bounds);

        inline const core::font_t* font_face() const {
            return font_family()->find_style(_font_style);
        }

    protected:
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
        dock::styles _dock = dock::styles::none;
        uint8_t _font_style = font::styles::normal;
        uint8_t _flags = config::flags::enabled | config::flags::visible;
    };

};

