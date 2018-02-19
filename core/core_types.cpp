//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <string>
#include "core_types.h"

namespace ryu::core {

    void element_t::safe_value(std::stringstream& stream) const {
        if (is_newline())
            stream << " ";
        else if (is_tab())
            stream << "    ";           // XXX: this is hard coded
        else if (is_percent())
            stream << "%%";
        else if (is_space())
            stream << " ";
        else
            stream << value;
    }

}