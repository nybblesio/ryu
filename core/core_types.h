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
#include <functional>
#include <fmt/format.h>
#include <SDL_FontCache.h>
#include <boost/variant.hpp>

namespace ryu::core {

    // --------------------
    // id pool
    // --------------------
    using id_t = uint32_t;

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

    using project_file_shared_ptr = std::shared_ptr<project_file>;
    typedef std::vector<project_file_shared_ptr> project_file_list;

    // --------------------
    // context
    // state
    // --------------------
    class context;

    typedef std::map<int, context*> context_dict;

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
    // colors & palettes
    // --------------------
    using palette_index = uint8_t;

    // --------------------
    // view
    // --------------------
    class view;

    using view_list = std::vector<view*>;

    struct pick_list_option_t {
        uint32_t key;
        std::string text;
    };

    using option_list = std::vector<pick_list_option_t>;

    struct alignment {
        struct horizontal {
            enum types : uint8_t {
                none,
                left,
                right,
                center
            };
        };

        struct vertical {
            enum types : uint8_t {
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
        enum types : uint8_t {
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

    using border_type = uint8_t;
    using dock_style = uint8_t;
    using span_list = std::vector<span_t>;
    using vertex_list = std::vector<vertex_t>;

    // --------------------
    // attributed text
    // --------------------
    struct attr_t {
        palette_index color = 0;
        uint8_t style = 0;
        uint8_t flags = 0;
        bool operator== (const attr_t& rhs) const {
            return color == rhs.color && style == rhs.style && flags == rhs.flags;
        }
        bool operator!= (const attr_t& rhs) const {
            return color != rhs.color || style != rhs.style || flags != rhs.flags;
        }
    };

    struct attr_chunk_t {
        attr_t attr;
        std::string text {};
    };

    typedef std::vector<attr_chunk_t> attr_chunks;

    using code_to_attr_callable = std::function<void (attr_t&)>;
    typedef std::map<std::string, code_to_attr_callable> code_to_attr_dict;

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
        core::alignment::horizontal::types alignment = core::alignment::horizontal::types::left;
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

    struct once_value_t {
        bool show = true;
        void add_once_column(
                data_table_row_t& row,
                const std::string& value) {
            if (show) {
                row.columns.push_back(value);
                show = false;
            } else {
                row.columns.emplace_back("");
            }
        }
    };

    // --------------------
    // ide
    // --------------------
    struct system_commands {
        enum types {
            unknown,

            quit,
            clear,
            edit_source,
            edit_memory,
            list_machine,
            edit_project,
            edit_machine,
            edit_component,
            edit_tiles,
            edit_sprites,
            edit_backgrounds,
            edit_music,
            edit_sounds,
            edit_palette,
            edit_actor,
            edit_cpu,

            read_text,
            write_text,
            find_text,
            goto_line,
            save_project_file,
            update_working_directory
        };
    };

    // --------------------
    // environment parameters
    // --------------------
    using parameter_variant_t = boost::variant<data_table_t, std::string, uint32_t, bool, system_commands::types>;

    enum parameter_dict_types {
        table = 0,
        string,
        integer32,
        boolean,
        system_command
    };

    using state_transition_command = uint32_t;

    using parameter_dict = std::map<std::string, parameter_variant_t>;
    using state_transition_callable = std::function<bool (state_transition_command, const parameter_dict&)>;

    using byte_list = std::vector<uint8_t>;
    using address_list = std::vector<uint32_t>;

    // --------------------
    // observable types
    // --------------------
    struct observables {
        enum types : uint32_t {
            unknown = 0,
            project
        };
    };

};