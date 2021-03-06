//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>
#include <rttr/registration>
#include <core/core_types.h>
#include <core/assembly_language_parser.h>
#include "memory_map.h"
#include "hardware_types.h"

namespace ryu::hardware {

    using access_type_flags = uint8_t;

    class integrated_circuit {
    public:
        struct endianness {
            enum types {
                none        = 0,
                big         = 1,
                little      = 2
            };
        };

        enum access_types {
            none        = 0b00000000,
            readable    = 0b00000001,
            writable    = 0b00000010,
        };

        explicit integrated_circuit(const std::string& name);

        virtual ~integrated_circuit() = default;

        void initialize();

        uint32_t id() const;

        virtual void zero();

        std::string name() const;

        bool write_latch() const;

        virtual uint16_t read_word(
                uint32_t address,
                integrated_circuit::endianness::types endianess) const;

        virtual uint32_t read_dword(
                uint32_t address,
                integrated_circuit::endianness::types endianess) const;

        void write_latch(bool enabled);

        uint32_t address_space() const;

        virtual void fill(uint8_t value);

        virtual uint32_t type_id() const;

        hardware::component* component();

        void address_space(uint32_t value);

        virtual ryu::core::byte_list write_word(
            uint32_t address,
            uint16_t value,
            integrated_circuit::endianness::types endianess);

        virtual ryu::core::byte_list write_dword(
            uint32_t address,
            uint32_t value,
            integrated_circuit::endianness::types endianess);

        virtual bool is_video_generator() const {
            return false;
        }

        virtual bool is_audio_generator() const {
            return false;
        }

        void component(hardware::component* value);

        virtual endianness::types endianess() const;

        virtual access_type_flags access_type() const;

        const hardware::memory_map& memory_map() const;

        virtual uint8_t read_byte(uint32_t address) const;

        virtual core::assembly_language_parser* assembler();

        virtual void write_byte(uint32_t address, uint8_t value);

        RTTR_ENABLE()

    protected:
        void clear_memory_map();

        void add_memory_map_entry(
                uint32_t offset,
                uint32_t size,
                const std::string& name,
                const std::string& description,
                memory_map_entry::memory_map_entry_flags flags = memory_map_entry::flags::none);

        void add_memory_map_entry(
                uint32_t offset,
                uint32_t size,
                const std::string& name,
                const std::string& description,
                const memory_map_entry::read_callable& reader,
                const memory_map_entry::write_callable& writer,
                memory_map_entry::memory_map_entry_flags flags = memory_map_entry::flags::none);

        virtual void on_initialize();

        virtual void on_address_space_changed();

    private:
        uint32_t _id {};
        std::string _name;
        bool _write_latch {};
        uint32_t _address_space;
        hardware::memory_map _memory_map;
        hardware::component* _component = nullptr;
    };

};
