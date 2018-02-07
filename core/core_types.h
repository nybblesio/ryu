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
    };

    struct data_table_t {
        std::vector<data_table_column_t> headers {};
        std::vector<data_table_column_t> footers {};
        std::vector<data_table_row_t> rows {};
        uint8_t line_spacing = 0;
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
};