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
#include "input_action_provider.h"

namespace ryu::core {

    class view_host {
    public:
        enum change_reasons : uint8_t {
            focus       = 0b00000001,
            visibility  = 0b00000010
        };

        using change_reason_flags = uint8_t;
        using state_change_callable = std::function<void (change_reason_flags)>;

        virtual ~view_host() = default;

        virtual bool is_focused() const = 0;

        virtual bool is_visible() const = 0;

        virtual void on_change(const state_change_callable& callable) = 0;
    };

    class view {
    public:
        struct sizing {
            enum types {
                content,
                fixed,
                parent
            };
        };

        struct config {
            enum flags : uint8_t {
                none     = 0b00000000,
                visible  = 0b00000001,
                enabled  = 0b00000010,
                tab_stop = 0b00000100,
                focused  = 0b00001000,
                layout   = 0b00010000
            };
        };

        struct types {
            enum id : uint8_t {
                control,
                container
            };
        };

        view(
            types::id type,
            const std::string& name,
            core::view_host* host);

        virtual ~view();

        void initialize();

        uint32_t id() const;

        bool layout() const;

        short index() const;

        core::rect& bounds();

        bool focused() const;

        bool enabled() const;

        bool visible() const;

        core::view* parent();

        bool tab_stop() const;

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

        std::string name() const;

        core::palette* palette();

        uint8_t bg_color() const;

        uint8_t fg_color() const;

        dock::styles dock() const;

        void tab_stop(bool value);

        uint8_t font_style() const;

        void dock(dock::styles style);

        void font_style(uint8_t styles);

        void add_child(core::view* child);

        void next_view(core::view* value);

        void prev_view(core::view* value);

        view::sizing::types sizing() const;

        virtual core::rect client_bounds();

        void draw(core::renderer& renderer);

        void focus(const core::view* target);

        void remove_child(core::view* child);

        virtual void bg_color(uint8_t value);

        virtual void fg_color(uint8_t value);

        void bounds(const core::rect& value);

        const core::font_t* font_face() const;

        void sizing(view::sizing::types value);

        core::font_family* font_family() const;

        core::view* get_child_at(size_t index);

        void margin(const core::padding& value);

        void padding(const core::padding& value);

        virtual void palette(core::palette* value);

        void resize(const core::rect& context_bounds);

        virtual void font_family(core::font_family* value);

        void update(uint32_t dt, core::pending_event_list& events);

    protected:
        view_host* host();

        core::view* find_root();

        void inner_focus(bool value);

        virtual void on_initialize();

        virtual void on_focus_changed();

        void listen_for_on_host_change();

        core::input_action_provider& action_provider();

        virtual void on_draw(core::renderer& renderer);

        virtual void draw_children(core::renderer& renderer);

        virtual void on_resize(const core::rect& context_bounds);

        virtual void on_update(uint32_t dt, core::pending_event_list& events);

    private:
        void bind_events();

        void define_actions();

        void on_host_changed(view_host::change_reason_flags flags);

    private:
        uint32_t _id;
        short _index = 0;
        std::string _name;
        core::rect _rect {};
        uint8_t _bg_color = 0;
        uint8_t _fg_color = 0;
        view_list _children {};
        core::padding _margin {};
        core::padding _padding {};
        core::view* _prev = nullptr;
        core::view* _next = nullptr;
        core::view* _parent = nullptr;
        types::id _type = types::control;
        core::view_host* _host = nullptr;
        core::palette* _palette = nullptr;
        core::font_family* _font = nullptr;
        uint8_t _font_style = font::styles::normal;
        core::dock::styles _dock = dock::styles::none;
        core::input_action_provider _action_provider {};
        view::sizing::types _sizing = view::sizing::types::content;
        uint8_t _flags = config::flags::enabled | config::flags::visible;
    };

};