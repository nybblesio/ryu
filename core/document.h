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

#include <map>
#include <list>
#include <core/views/caret.h>
#include <boost/filesystem.hpp>
#include "result.h"
#include "piece_table.h"

namespace ryu::core {

    namespace fs = boost::filesystem;

    class document {
    public:
        using document_changed_callable = std::function<void ()>;

        document() = default;

        ~document() = default;

        void home();

        void clear();

        void page_up();

        void shift_up();

        bool scroll_up();

        void page_down();

        void last_page();

        element_t* get();

        void first_page();

        void split_line();

        bool scroll_down();

        bool scroll_left();

        void delete_line();

        void insert_line();

        bool scroll_right();

        int32_t row() const;

        core::caret* caret();

        bool is_line_empty();

        fs::path path() const;

        uint32_t rows() const;

        bool row(uint32_t row);

        int16_t column() const;

        uint16_t find_line_end();

        uint16_t columns() const;

        void end(uint16_t column);

        uint8_t page_width() const;

        uint8_t page_height() const;

        bool column(uint16_t column);

        void caret(core::caret* value);

        void path(const fs::path& value);

        void put(const element_t& value);

        attr_span_list line_at(uint32_t row);

        void shift_left(uint16_t times = 1);

        void shift_right(uint16_t times = 1);

        void shift_line_left(uint16_t times = 1);

        void shift_line_right(uint16_t times = 1);

        void page_size(uint8_t height, uint8_t width);

        void document_size(uint32_t rows, uint16_t columns);

        bool load(core::result& result, const fs::path& path);

        bool load(core::result& result, std::istream& stream);

        bool save(core::result& result, std::ostream& stream);

        bool save(core::result& result, const fs::path& path = "");

        void on_document_changed(const document_changed_callable& callable);

    protected:
        bool clamp_row();

        bool clamp_column();

        void raise_document_changed();

    private:
        fs::path _path;
        int32_t _row = 0;
        uint32_t _rows = 1;
        int16_t _column = 0;
        uint16_t _columns = 80;
        uint8_t _page_width = 0;
        uint8_t _page_height = 0;
        piece_table_t _piece_table {};
        core::caret* _caret = nullptr;
        document_changed_callable _document_changed_callback;
    };

}