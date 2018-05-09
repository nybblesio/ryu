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
#include "panel.h"

namespace ryu::core {

    class document_footer : public core::panel {
    public:
        document_footer(
            const std::string& name,
            core::view_host* host);

        ~document_footer() override;

        void update_state(
            const core::caret& caret,
            const core::document& document);

    protected:
        void on_initialize() override;

        void on_palette_changed() override;

        void on_font_family_changed() override;

    private:
        core::label _caret_status;
        core::label _document_status;
    };

};

