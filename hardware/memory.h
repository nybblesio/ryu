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

namespace ryu::hardware {

    class memory {
    public:
        virtual ~memory() = default;

        virtual size_t size() const = 0;
    };

}