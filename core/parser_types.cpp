//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "parser_types.h"

namespace ryu::core {

    std::map<std::string, directive_t::types> directive_t::_directive_to_type = {
            // assembler control directives
            {"org",    directive_t::types::origin},
            {"target", directive_t::types::target},
            {"equ",    directive_t::types::equate},

            // data directives
            {"align",  directive_t::types::align},
            {"byte",   directive_t::types::data},
            {"word",   directive_t::types::data},
            {"dword",  directive_t::types::data},
            {"ascii",  directive_t::types::data},
            {"struct", directive_t::types::structure},

            // macros, conditionals, loops
            {"if",     directive_t::types::if_block},
            {"elseif", directive_t::types::elseif_block},
            {"else",   directive_t::types::else_block},
            {"loop",   directive_t::types::loop},
            {"macro",  directive_t::types::macro},

            // block/scope
            {"local",  directive_t::types::local},
            {"end",    directive_t::types::end_block},
    };

}