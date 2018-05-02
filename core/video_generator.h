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

#include <cstdint>
#include <rttr/registration>
#include "size.h"

namespace ryu::core {

    struct tile_encoding {
        enum types {
            indexed,
            nybble_indexed,
            rle
        };
    };

    struct tile_attributes_t {
        size dimensions;
        uint32_t address;
        tile_encoding::types encoding;
    };

    struct palette_attributes_t {
        uint32_t address;
        uint8_t number_of_entries;
        uint8_t number_of_palettes;
    };

    struct background_attributes_t {
        uint8_t layers;
        size dimensions;
        uint32_t address;
    };

    class video_generator {
    public:
        virtual ~video_generator() = default;

        virtual tile_attributes_t tile_attributes() const = 0;

        virtual tile_attributes_t sprite_attributes() const = 0;

        virtual palette_attributes_t palette_attributes() const = 0;

        virtual background_attributes_t background_attributes() const = 0;

        RTTR_ENABLE()
    };
}