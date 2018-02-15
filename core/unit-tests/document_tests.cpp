//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <catch2.hpp>
#include <core/document.h>

namespace ryu::core::unit_tests {

    TEST_CASE("document_returns_text", "[document][!hide]") {
        core::caret cursor("caret");
        cursor.initialize(0, 0);

        core::document doc;
        doc.caret(&cursor);
        doc.page_size(25, 80);

        attr_t default_attr(1, 0, 0);

        std::string expected_text = "this is a test";
        doc.put(element_list_t::from_string(default_attr, expected_text));
        doc.rebuild();

        auto lines = doc.lines_from(0);
        REQUIRE(!lines.empty());
    }

}