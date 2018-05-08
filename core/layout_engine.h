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

#include <layout.h>
#include "size.h"

namespace ryu::core {

    class layout_engine {
    public:
        layout_engine();

        virtual ~layout_engine();

        void reset();

        inline lay_id root() const {
            return _root;
        }

        void size(const core::size& value);

    private:
        lay_id _root;
        lay_context _context;
    };

};

