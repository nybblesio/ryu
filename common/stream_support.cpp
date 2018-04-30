//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <fmt/format.h>
#include "stream_support.h"

namespace ryu {

    bool read_text(
            core::result& result,
            const fs::path& path,
            std::iostream& stream) {
        try {
            std::ifstream file(path.string(), std::ios::in);
            if (file.is_open()) {
                stream << file.rdbuf();
                file.close();
            }
        } catch (std::exception& e) {
            result.add_message(
                "P001",
                fmt::format("unable to read_text from file: {}", e.what()),
                true);
            return false;
        }
        return true;
    }

    bool read_binary(
            core::result& result,
            const fs::path& path,
            std::iostream& stream) {
        try {
            std::ifstream file(
                path.string(),
                std::ios::in|std::ios::binary);
            if (file.is_open()) {
                stream << file.rdbuf();
                file.close();
            }
        } catch (std::exception& e) {
            result.add_message(
                "P001",
                fmt::format("unable to read_binary from file: {}", e.what()),
                true);
            return false;
        }
        return true;
    }

    bool write_text(
            core::result& result,
            const fs::path& path,
            const std::iostream& stream) {
        try {
            std::ofstream file(path.string(), std::ios::out);
            if (file.is_open()) {
                file << stream.rdbuf();
                file.close();
            }
        } catch (std::exception& e) {
            result.add_message(
                "P001",
                fmt::format("unable to write_text to file: {}", e.what()),
                true);
            return false;
        }
        return true;
    }

    bool write_text(
            core::result& result,
            const fs::path& path,
            const std::string& text) {
        try {
            std::ofstream file(path.string(), std::ios::out);
            if (file.is_open()) {
                file << text;
                file.close();
            }
        } catch (std::exception& e) {
            result.add_message(
                "P001",
                fmt::format("unable to write_text to file: {}", e.what()),
                true);
            return false;
        }
        return true;
    }

    bool write_binary(
            core::result& result,
            const fs::path& path,
            const std::iostream& stream) {
        try {
            std::ofstream file(
                path.string(),
                std::ios::out|std::ios::binary);
            if (file.is_open()) {
                file << stream.rdbuf();
                file.close();
            }
        } catch (std::exception& e) {
            result.add_message(
                "P001",
                fmt::format("unable to write_binary to file: {}", e.what()),
                true);
            return false;
        }
        return true;
    }

}