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

#include <core/result.h>
#include <boost/filesystem.hpp>

namespace ryu {

    namespace fs = boost::filesystem;

    bool read_text(
        core::result& result,
        const fs::path& path,
        std::iostream& stream);

    bool write_text(
        core::result& result,
        const fs::path& path,
        const std::iostream& stream);

    bool write_text(
        core::result& result,
        const fs::path& path,
        const std::string& text);

    bool read_binary(
        core::result& result,
        const fs::path& path,
        std::iostream& stream);

    bool write_binary(
        core::result& result,
        const fs::path& path,
        const std::iostream& stream);

};

