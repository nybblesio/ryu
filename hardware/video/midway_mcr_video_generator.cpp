//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "midway_mcr_video_generator.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::midway_mcr_video_generator>("ryu::hardware::midway_mcr_video_generator") (
            rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::video_generator_id),
            rttr::metadata(ryu::hardware::meta_data_key::type_name, "Midway MCR II/III video generator"))
        .constructor<>(rttr::registration::public_access) (
            rttr::policy::ctor::as_raw_ptr
        );
}

namespace ryu::hardware {

    void midway_mcr_video_generator::init() {
    }

    midway_mcr_video_generator::midway_mcr_video_generator() : integrated_circuit("mcr-video-generator") {
    }

    void midway_mcr_video_generator::zero() {
    }

    void midway_mcr_video_generator::fill(uint8_t value) {
    }

    std::vector<uint8_t> midway_mcr_video_generator::write_word(
            uint32_t address,
            uint16_t value,
            integrated_circuit::endianness::types endianess) {
        return {};
    }

    std::vector<uint8_t> midway_mcr_video_generator::write_dword(
            uint32_t address,
            uint32_t value,
            integrated_circuit::endianness::types endianess) {
        return {};
    }

    void midway_mcr_video_generator::on_address_space_changed() {
        zero();
    }

    uint16_t midway_mcr_video_generator::read_word(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        return 0;
    }

    uint32_t midway_mcr_video_generator::read_dword(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        return 0;
    }

    access_type_flags midway_mcr_video_generator::access_type() const {
        return access_types::writable | access_types::readable;
    }

    uint8_t midway_mcr_video_generator::read_byte(uint32_t address) const {
        return 0;
    }

    core::palette_list midway_mcr_video_generator::decode_palettes() const {
        core::palette_list palettes {};
        return palettes;
    }

    core::tile_attributes_t midway_mcr_video_generator::tile_attributes() const {
        return core::tile_attributes_t { {16, 16}, 0 };
    }

    void midway_mcr_video_generator::write_byte(uint32_t address, uint8_t value) {
    }

    core::tile_attributes_t midway_mcr_video_generator::sprite_attributes() const {
        return core::tile_attributes_t { {32, 32}, 0 };
    }

    core::palette_attributes_t midway_mcr_video_generator::palette_attributes() const {
        return core::palette_attributes_t { 0, 16, 4 };
    }

    void midway_mcr_video_generator::encode_palettes(const core::palette_list& palettes) {

    }

    core::background_attributes_t midway_mcr_video_generator::background_attributes() const {
        return core::background_attributes_t { 1, {32, 30}, 0 };
    }

}