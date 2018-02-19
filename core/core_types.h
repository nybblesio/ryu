//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <map>
#include <list>
#include <stack>
#include <fmt/format.h>
#include <boost/variant.hpp>
#include <common/SDL_FontCache.h>

namespace ryu::core {

    // --------------------
    // timers
    // --------------------
    class timer;

    typedef std::vector<timer*> timer_list;
    typedef std::function<void (timer*)> timer_callable;

    // --------------------
    // engine
    // --------------------
    class engine;

    // --------------------
    // project & project_file
    // --------------------
    class project;

    typedef std::shared_ptr<core::project> project_shared_ptr;
    typedef std::map<std::string, std::string> project_props_dict;

    class project_file;

    typedef std::vector<project_file> project_file_list;

    // --------------------
    // context
    // state
    // --------------------
    class context;

    typedef std::map<int, context*> context_dict;
    typedef std::vector<SDL_Event> event_list;

    struct context_window {
        enum sizes {
            split,
            expanded,
            collapsed
        };
    };

    class state;

    struct state_t {
        state* instance;
    };

    typedef std::map<int, state_t> state_dict;
    typedef std::map<std::string, std::string> blackboard;

    // --------------------
    // view
    // --------------------
    class view;

    typedef std::vector<view*> view_list;

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

    struct vertex_t {
        int16_t x {};
        int16_t y {};
    };

    struct span_t {
        int16_t left {};
        int16_t right {};
    };

    typedef std::vector<span_t> span_list;
    typedef std::vector<vertex_t> vertex_list;

    // --------------------
    // data tables
    // --------------------
    struct data_table_column_t {
        enum format_options : uint8_t {
            none        = 0b00000000,
            style_codes = 0b00000001,
            word_wrap   = 0b00000010,
        };

        std::string text {};
        uint16_t min_width {};
        uint16_t max_width {};
        alignment::horizontal::types alignment = alignment::horizontal::types::left;
        uint8_t padding = 1;
        uint8_t options = format_options::none;
        uint16_t width {};
    };

    struct data_table_row_t {
        std::vector<std::string> columns {};

        void add_empty_column() {
            columns.emplace_back("");
        }
    };

    struct data_table_t {
        std::vector<data_table_column_t> headers {};
        std::vector<data_table_column_t> footers {};
        std::vector<data_table_row_t> rows {};
        uint8_t line_spacing = 0;

        void add_empty_line() {
            data_table_row_t row {};
            for (auto i = 0; i < headers.size(); i++)
                row.columns.emplace_back("");
            rows.push_back(row);
        }
    };

    // --------------------
    // environment parameters
    // --------------------
    typedef boost::variant<
            data_table_t,
            std::string,
            uint32_t,
            bool> parameter_variant_t;

    enum parameter_dict_types {
        table = 0,
        string,
        integer32,
        boolean
    };

    typedef std::map<std::string, parameter_variant_t> parameter_dict;
    typedef std::function<bool (const std::string&, const parameter_dict&)> state_transition_callable;

    typedef std::vector<uint8_t> byte_list;
    typedef std::vector<uint32_t> address_list;

    // --------------------
    // attr_t, element_t
    //   and related types
    // --------------------

    struct attr_t {
        attr_t() = default;

        attr_t(uint8_t color, uint8_t style, uint8_t flags) : color(color),
                                                              style(style),
                                                              flags(flags) {
        }

        attr_t(const attr_t& other) : color(other.color),
                                      style(other.style),
                                      flags(other.flags) {
        }

        uint8_t color = 0;
        uint8_t style = 0;
        uint8_t flags = 0;
        bool operator== (const attr_t& rhs) const {
            return color == rhs.color && style == rhs.style && flags == rhs.flags;
        }
        bool operator!= (const attr_t& rhs) const {
            return color != rhs.color || style != rhs.style || flags != rhs.flags;
        }
    };

    struct attr_span_t {
        attr_t attr;
        std::string text {};
    };

    using attr_span_list = std::vector<attr_span_t>;

    struct element_t {
        attr_t attr;
        uint8_t value = 0;

        inline bool is_tab() const {
            return value == '\t';
        }

        inline bool is_space() const {
            return isspace(value);
        }

        inline bool is_newline() const {
            return value == '\n';
        }

        inline bool is_percent() const {
            return value == 37;
        }

        void safe_value(std::stringstream& stream) const;
    };

    using element_list = std::vector<element_t>;

    struct element_list_t {
    public:
        static element_list_t from_string(
                const attr_t& attr,
                const std::string& value) {
            element_list_t elements{};
            for (const auto& c: value)
                elements.push_back(element_t{attr, static_cast<uint8_t>(c)});
            return elements;
        }

        inline bool empty() const {
            return _elements.empty();
        }

        inline size_t size() const {
            return _elements.size();
        }

        const element_t& back() const {
            return _elements.back();
        }

        const element_t& front() const {
            return _elements.front();
        }

        element_list::iterator end() {
            return _elements.end();
        }

        element_list::iterator begin() {
            return _elements.begin();
        }

        element_t& operator[](size_t index) {
            return _elements[index];
        }

        void push_back(const element_t& value) {
            _elements.push_back(value);
        }

        element_list::const_iterator end() const {
            return _elements.cend();
        }

        element_list::const_iterator begin() const {
            return _elements.cbegin();
        }

    private:
        element_list _elements {};
    };

    struct attr_span_list_t {
        attr_span_t& back() {
            return _spans.back();
        }

        inline bool empty() const {
            return _spans.empty();
        }

        inline size_t size() const {
            return _spans.size();
        }

        attr_span_list::iterator end() {
            return _spans.end();
        }

        element_list_t sequence() {
            element_list_t elements {};
            for (const auto& span : _spans) {
                for (auto c : span.text)
                    elements.push_back(element_t{span.attr, static_cast<uint8_t>(c)});
            }
            return elements;
        }

        const attr_span_t& back() const {
            return _spans.back();
        }

        attr_span_list::iterator begin() {
            return _spans.begin();
        }

        attr_span_t& operator[](size_t index) {
            return _spans[index];
        }

        attr_span_list::const_iterator end() const {
            return _spans.cend();
        }

        attr_span_list::const_iterator begin() const {
            return _spans.cbegin();
        }

        inline void push_back(const attr_span_t& span) {
            _spans.push_back(span);
        }

        const attr_span_t& operator[](size_t index) const {
            return _spans[index];
        }

        inline void insert(const attr_span_list::iterator& it, const attr_span_t& span) {
            _spans.insert(it, span);
        }

        attr_span_list _spans {};
    };

    using attr_line_list = std::vector<attr_span_list_t>;

    using code_to_attr_callable = std::function<void (attr_t&)>;
    using code_to_attr_dict = std::map<std::string, code_to_attr_callable>;

};