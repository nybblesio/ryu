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

#include <core/document.h>
#include "label.h"
#include "caret.h"
#include "dock_layout_panel.h"

namespace ryu::core {

    class document_footer : public core::dock_layout_panel {
    public:
        document_footer(
            const std::string& name,
            core::view_host* host);

        void value(
            const core::caret& caret,
            const core::document& document);

    protected:
        void on_initialize() override;

        void on_resize(const core::rect& context_bounds) override;

    private:
        core::label _caret_status;
        core::label _document_status;
    };

};

