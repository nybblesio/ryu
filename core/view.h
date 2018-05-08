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
#include "rect.h"
#include "palette.h"
#include "context.h"
#include "padding.h"
#include "renderer.h"
#include "font_family.h"
#include "layout_engine.h"
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

        virtual core::preferences* prefs() = 0;

        virtual core::layout_engine* layout_engine() = 0;

        virtual void remove_change_listener(id_t id) = 0;

        virtual id_t add_change_listener(const state_change_callable& callable) = 0;
    };

    class view {
    public:
        struct config {
            enum flags : uint8_t {
                none     = 0b00000000,
                visible  = 0b00000001,
                enabled  = 0b00000010,
                tab_stop = 0b00000100,
                focused  = 0b00001000,
                clip     = 0b00010000,
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

        id_t id() const;

        void initialize();

        core::rect& bounds();

        bool focused() const;

        bool enabled() const;

        bool visible() const;

        core::view* parent();

        bool tab_stop() const;

        void clear_children();

        view_list& children();

        uint16_t index() const;

        types::id type() const;

        core::size& min_size();

        core::padding& margin();

        core::padding& padding();

        void enabled(bool value);

        void visible(bool value);

        std::string name() const;

        core::palette* palette();

        uint8_t bg_color() const;

        uint8_t fg_color() const;

        void tab_stop(bool value);

        bool should_clip() const;

        void index(uint16_t value);

        uint8_t font_style() const;

        void should_clip(bool value);

        template <typename T>
        T* find_by_id(uint32_t id) {
            if (_id == id)
                return dynamic_cast<T*>(this);
            for (auto child : _children) {
                auto found = child->find_by_id<T>(id);
                if (found != nullptr)
                    return found;
            }
            return nullptr;
        }

        core::dock::styles dock() const;

        void font_style(uint8_t styles);

        void add_child(core::view* child);

        void next_view(core::view* value);

        void prev_view(core::view* value);

        virtual std::string value() const;

        virtual core::rect inner_bounds();

        core::border::types border() const;

        void draw(core::renderer& renderer);

        void dock(core::dock::styles style);

        void focus(const core::view* target);

        void remove_child(core::view* child);

        virtual void bg_color(uint8_t value);

        virtual void fg_color(uint8_t value);

        void bounds(const core::rect& value);

        const core::font_t* font_face() const;

        core::font_family* font_family() const;

        core::view* get_child_at(size_t index);

        void border(core::border::types value);

        void margin(const core::padding& value);

        void padding(const core::padding& value);

        virtual void palette(core::palette* value);

        template <typename T>
        T* find_by_name(const std::string& name) {
            if (_name == name)
                return dynamic_cast<T*>(this);
            for (view* child : _children) {
                auto found = child->find_by_name<T>(name);
                if (found != nullptr)
                    return found;
            }
            return nullptr;
        }

        virtual void value(const std::string& value);

        virtual void font_family(core::font_family* value);

        void update(uint32_t dt, core::pending_event_list& events);

    protected:
        view_host* host();

        core::view* find_root();

        void inner_focus(bool value);

        virtual void on_initialize();

        virtual void on_focus_changed();

        void listen_for_on_host_change();

        virtual void on_bounds_changed();

        virtual void on_palette_changed();

        core::input_action_provider& action_provider();

        virtual void on_draw(core::renderer& renderer);

        virtual void on_update(uint32_t dt, core::pending_event_list& events);

    private:
        void bind_events();

        void define_actions();

        void render_list_sort();

        void render_list_build();

        void render_list_root_reset();

        void on_host_changed(view_host::change_reason_flags flags);

    private:
        id_t _id;
        std::string _name;
        std::string _value;
        core::rect _rect {};
        uint16_t _index = 0;
        id_t _host_callback_id;
        view_list _children {};
        core::size _min_size {};
        core::padding _margin {};
        view_list _render_list {};
        core::padding _padding {};
        core::view* _prev = nullptr;
        core::view* _next = nullptr;
        palette_index _bg_color = 0;
        palette_index _fg_color = 0;
        core::view* _parent = nullptr;
        types::id _type = types::control;
        core::view_host* _host = nullptr;
        core::palette* _palette = nullptr;
        core::font_family* _font = nullptr;
        uint8_t _font_style = font::styles::normal;
        border::types _border = border::types::none;
        core::dock::styles _dock = dock::styles::none;
        core::input_action_provider _action_provider {};
        uint8_t _flags = config::flags::enabled | config::flags::visible;
    };

};