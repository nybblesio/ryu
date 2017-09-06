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

namespace ryu::cpu {

    struct models {
        enum ids {
            none = 0,
            mc6809 = 1,
        };
    };

    class model;

    typedef std::vector<model> model_list;

}