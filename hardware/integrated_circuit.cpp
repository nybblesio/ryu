//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#include <rttr/registration>
#include "integrated_circuit.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::integrated_circuit>("ryu::hardware::integrated_circuit") (
        rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::integrated_circuit_id),
        rttr::metadata(ryu::hardware::meta_data_key::type_name, "Base IC")
    )
    .constructor<const std::string&>(rttr::registration::public_access) (
            rttr::policy::ctor::as_raw_ptr
    )
    .property_readonly("id", &ryu::hardware::integrated_circuit::id)
    .property_readonly("name", &ryu::hardware::integrated_circuit::name);
}

namespace ryu::hardware {

    integrated_circuit::integrated_circuit(const std::string& name) : _id(core::id_pool::instance()->allocate()),
                                                                      _name(name) {
    }

    void integrated_circuit::zero() {
    }

    bool integrated_circuit::write_latch() const {
        return _write_latch;
    }

    void integrated_circuit::write_latch(bool enabled) {
        _write_latch = enabled;
    }

    uint32_t integrated_circuit::id() const {
        return _id;
    }

    uint16_t integrated_circuit::read_word(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        return 0;
    }

    uint32_t integrated_circuit::read_dword(
            uint32_t address,
            integrated_circuit::endianness::types endianess) const {
        return 0;
    }

    ryu::core::byte_list integrated_circuit::write_word(
            uint32_t address,
            uint16_t value,
            integrated_circuit::endianness::types endianess) {
        return {};
    }

    ryu::core::byte_list integrated_circuit::write_dword(
            uint32_t address,
            uint32_t value,
            integrated_circuit::endianness::types endianess) {
        return {};
    }

    void integrated_circuit::clear_memory_map() {
        _memory_map.clear();
    }

    void integrated_circuit::fill(uint8_t value) {
    }

    std::string integrated_circuit::name() const {
        return _name;
    }

    void integrated_circuit::add_memory_map_entry(
            uint32_t offset,
            uint32_t size,
            const std::string& name,
            const std::string& description) {
        _memory_map.add(offset, size, name, description);
    }

    void integrated_circuit::add_memory_map_entry(
            uint32_t offset,
            uint32_t size,
            const std::string& name,
            const std::string& description,
            const memory_map_entry::read_callable& reader,
            const memory_map_entry::write_callable& writer) {
        _memory_map.add(offset, size, name, description, reader, writer);
    }

    uint32_t integrated_circuit::address_space() const {
        return _address_space;
    }

    void integrated_circuit::on_address_space_changed() {
    }

    std::string integrated_circuit::component_name() const {
        return _component_name;
    }

    void integrated_circuit::address_space(uint32_t value) {
        if (value != _address_space) {
            _address_space = value;
            on_address_space_changed();
        }
    }

    access_type_flags integrated_circuit::access_type() const {
        return access_types::none;
    }

    uint8_t integrated_circuit::read_byte(uint32_t address) const {
        return 0;
    }

    core::assembly_language_parser* integrated_circuit::assembler() {
        return nullptr;
    }

    void integrated_circuit::component_name(const std::string& name) {
        _component_name = name;
    }

    const hardware::memory_map& integrated_circuit::memory_map() const {
        return _memory_map;
    }

    void integrated_circuit::write_byte(uint32_t address, uint8_t value) {
    }

    integrated_circuit::endianness::types integrated_circuit::endianess() const {
        return endianness::types::none;
    }

}