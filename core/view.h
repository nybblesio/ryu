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
#include "font_family.h"

namespace ryu::core {

    struct alignment {
        struct horizontal {
            enum types {
                none,
                left,
                right,
                center
            };
        };

        struct vertical {
            enum types {
                none,
                top,
                middle,
                bottom
            };
        };

        static FC_AlignEnum to_font_align(horizontal::types value) {
            FC_AlignEnum align = FC_AlignEnum::FC_ALIGN_LEFT;
            switch (value) {
                case horizontal::none:
                case horizontal::left:
                    align = FC_AlignEnum::FC_ALIGN_LEFT;
                    break;
                case horizontal::right:
                    align = FC_AlignEnum::FC_ALIGN_RIGHT;
                    break;
                case horizontal::center:
                    align = FC_AlignEnum::FC_ALIGN_CENTER;
                    break;
            }
            return align;
        }
    };

    struct border {
        enum types {
            none,
            solid,
            dashed,
            rounded
        };
    };

    class view;

    typedef std::vector<view*> view_list;

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

        view(
                core::context* context,
                core::view* parent,
                types::id type,
                std::string name);

        virtual ~view();

        void draw();

        void resize();

        int id() const;

        view* parent();

        void focus(int id);

        short index() const;

        core::rect& bounds();

        bool focused() const;

        bool enabled() const;

        bool visible() const;

        bool tabstop() const;

        view_list& children();

        types::id type() const;

        core::padding& margin();

        core::padding& padding();

        void index(short value);

        void enabled(bool value);

        void visible(bool value);

        void tabstop(bool value);

        std::string name() const;

        core::context* context();

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

        void bounds(const core::rect& value);

        void palette(core::palette* palette);

        bool process_event(const SDL_Event* e);

        core::font_family* font_family() const;

        void margin(const core::padding& value);

        void padding(const core::padding& value);

        void font_family(core::font_family* font);

        void on_tab(const on_tab_callable& callable);

        inline const core::font_t* font_face() const {
            return font_family()->find_style(_font_style);
        }

    protected:
        void pop_blend_mode();

        virtual void on_draw();

        void focus(bool value);

        virtual void on_resize();

        virtual void on_focus_changed();

        void draw_text_aligned(
                const std::string& value,
                const core::rect& bounds,
                alignment::horizontal::types halign,
                alignment::vertical::types valign);

        void draw_rect(const core::rect& bounds);

        void fill_rect(const core::rect& bounds);

        void push_blend_mode(SDL_BlendMode mode);

        int measure_text(const std::string& value);

        void draw_line(int x1, int y1, int x2, int y2);

        void set_color(const core::palette_entry& color);

        virtual bool on_process_event(const SDL_Event* e);

        void set_font_color(const core::palette_entry& color);

        void draw_text(int x, int y, const std::string& value, const core::palette_entry& color);

    private:
        int _id;
        short _index;
        uint8_t _flags;
        std::string _name;
        core::rect _rect {};
        view_list _children;
        core::padding _margin;
        uint8_t _bg_color = 0;
        uint8_t _fg_color = 0;
        core::padding _padding;
        core::view* _parent = nullptr;
        types::id _type = types::control;
        on_tab_callable _on_tab_callable;
        bool _in_on_focus_changed = false;
        core::context* _context = nullptr;
        core::palette* _palette = nullptr;
        core::font_family* _font = nullptr;
        std::stack<SDL_BlendMode> _mode_stack;
        dock::styles _dock = dock::styles::none;
        uint8_t _font_style = font::styles::normal;
    };

};

