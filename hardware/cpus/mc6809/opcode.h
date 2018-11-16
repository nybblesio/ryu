//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <set>

namespace ryu::hardware::mc6809 {

    using mnemonic_set = std::set<std::string>;

    class opcode {
    public:
        enum address_modes : uint8_t {
            none        = 0b00000000,
            immediate   = 0b00000001,
            direct      = 0b00000010,
            indexed     = 0b00000100,
            extended    = 0b00001000,
            inherent    = 0b00010000,
            relative    = 0b00100000
        };

        opcode() = default;

        opcode(
                uint16_t op,
                uint8_t cycles,
                uint8_t mode,
                const mnemonic_set& mnemonics);

        opcode(const opcode& other) : _op(other._op),
                                      _cycles(other._cycles),
                                      _mnemonics(other._mnemonics),
                                      _mode(other._mode) {
        }

        opcode& operator= (const opcode& other) {
            _op = other._op;
            _mode = other._mode;
            _cycles = other._cycles;
            _mnemonics = other._mnemonics;
            return *this;
        }

        const mnemonic_set& mnemonics() const;

    private:
        uint16_t _op = 0;
        uint8_t _cycles = 0;
        mnemonic_set _mnemonics;
        uint8_t _mode = address_modes::none;
    };

    using opcode_dictionary = std::map<uint16_t, opcode>;
};
