//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <core/id_pool.h>
#include "display.h"

namespace ryu::hardware {

    // XXX: width, height, refresh rate need to be corrected
    display_list display::_displays = {
        { 1, "Atari 19\"",                      0, 0, 60},
        { 2, "Atari 25\"",                      0, 0, 60},
        { 3, "Bally/Midway",                    0, 0, 60},
        { 4, "Eastern Micro Electronics, Inc",  0, 0, 60},
        { 5, "Electrohome",                     0, 0, 60},
        { 6, "Eygo 814",                        0, 0, 60},
        { 7, "Eygo 820",                        0, 0, 60},
        { 8, "Eygo 822",                        0, 0, 60},
        { 9, "Happ Controls KJ-XX15 13\"",      0, 0, 60},
        {10, "Happ Controls KJ-XX15 19\"",      0, 0, 60},
        {11, "Hantarex MT-3000 9\"",            0, 0, 60},
        {12, "Hantarex MT-3000 12\"",           0, 0, 60},
        {13, "Hantarex MTC-900/E",              0, 0, 60},
        {14, "Hantarex MTC-9000",               0, 0, 60},
        {15, "Hantarex MTC-9110",               0, 0, 60},
        {16, "Kortek Imperial 19\"",            0, 0, 60},
        {17, "LAI LAI-KZ-14XYB",                0, 0, 60},
        {18, "LAI LAI-KZ-20XYB",                0, 0, 60},
        {19, "Matsushita 14\"",                 0, 0, 60},
        {20, "Matsushita TM-202G",              0, 0, 60},
        {21, "Motorola M5000 19\"",             0, 0, 60},
        {22, "Motorola M7000 23\"",             0, 0, 60},
        {23, "Motorola XM501-10 19\"",          0, 0, 60},
        {24, "Motorola XM701-10 23\"",          0, 0, 60},
        {25, "Nanao MC-2000-S",                 0, 0, 60},
        {26, "Neotec NT-2515C / N25E 25\"",     0, 0, 60},
        {27, "QNIC 14\"",                       0, 0, 60},
        {28, "QNIC 20\"",                       0, 0, 60},
        {29, "QNIC 25\"",                       0, 0, 60},
        {30, "Sanyo 14\"",                      0, 0, 60},
        {31, "Sanyo 20-EZV",                    0, 0, 60},
        {32, "Sanyo 20-Z2AW",                   0, 0, 60},
        {33, "Sharp XM-2001N",                  0, 0, 60},
        {34, "Sharp Image SI-3195 Sony 19\"",   0, 0, 60},
        {35, "Sharp Image SI-327 27\"",         0, 0, 60},
        {36, "TEC TM-600 19\"",                 0, 0, 60},
        {37, "TEC TM623 23\"",                  0, 0, 60},
        {38, "Wells-Gardner Raster",            0, 0, 60},
        {39, "Williams Raster",                 0, 0, 60},
        {40, "Zenith CD-19MRF06",               0, 0, 60},
    };

    display::display(
            uint32_t id,
            const std::string& name,
            uint16_t width,
            uint16_t height,
            uint8_t refresh_rate) : _id(id),
                                    _name(name),
                                    _width(width),
                                    _height(height),
                                    _refresh_rate(refresh_rate) {
        core::id_pool::instance()->mark_used(id);
    }

    uint32_t display::id() const {
        return _id;
    }

    uint16_t display::width() const {
        return _width;
    }

    uint16_t display::height() const {
        return _height;
    }

    display_list& display::catalog() {
        return _displays;
    }

    std::string display::name() const {
        return _name;
    }

    uint8_t display::refresh_rate() const {
        return _refresh_rate;
    }

}
