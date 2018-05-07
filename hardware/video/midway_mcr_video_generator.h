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

#include <core/video_generator.h>
#include <hardware/integrated_circuit.h>

namespace ryu::hardware {

    class midway_mcr_video_generator : public hardware::integrated_circuit,
                                       public core::video_generator {
    public:
        static void init();

        midway_mcr_video_generator();

        void zero() override;

        void fill(uint8_t value) override;

        uint32_t type_id() const override {
            return ryu::hardware::video_generator_id;
        }

        bool is_video_generator() const override {
            return true;
        }

        access_type_flags access_type() const override;

        uint8_t read_byte(uint32_t address) const override;

        uint16_t read_word(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const override;

        uint32_t read_dword(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const override;

        void write_byte(uint32_t address, uint8_t value) override;

        std::vector<uint8_t> write_word(
            uint32_t address,
            uint16_t value,
            integrated_circuit::endianness::types endianess) override;

        std::vector<uint8_t> write_dword(
            uint32_t address,
            uint32_t value,
            integrated_circuit::endianness::types endianess) override;

        core::palette_list decode_palettes() const override;

        core::tile_attributes_t tile_attributes() const override;

        core::tile_attributes_t sprite_attributes() const override;

        core::palette_attributes_t palette_attributes() const override;

        void encode_palettes(const core::palette_list& palettes) override;

        core::background_attributes_t background_attributes() const override;

    protected:
        void on_address_space_changed() override;

        RTTR_ENABLE(hardware::integrated_circuit, core::video_generator)

    private:
    };

};