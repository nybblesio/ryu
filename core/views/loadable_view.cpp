//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "loadable_view.h"

namespace ryu::core {

    loadable_view::loadable_view(
            const std::string& name,
            core::view_host* host) : dock_layout_panel(name, host) {
    }

    loadable_view::~loadable_view() {

    }

    bool loadable_view::load(
            core::result& result,
            const boost::filesystem::path& path) {
        return !result.is_failed();
    }

    void loadable_view::on_initialize() {
    }

}