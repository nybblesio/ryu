//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "document_footer.h"

namespace ryu::core {

    document_footer::document_footer(
            const std::string& name,
            core::view_host* host) : dock_layout_panel(name, host),
                                     _caret_status("caret-status-label", host),
                                     _document_status("document-status-label", host) {
    }

    document_footer::~document_footer() {
    }

    void document_footer::update_state(
            const core::caret& caret,
            const core::document& document) {
        _document_status.value(fmt::format(
                "C:{:03d}/{:03d} R:{:04d}/{:04d}",
                document.column() + caret.column() + 1,
                document.columns(),
                document.row() + caret.row() + 1,
                document.rows()));
        _caret_status.value(fmt::format(
                "| X:{:03d} Y:{:02d} | {}",
                caret.column() + 1,
                caret.row() + 1,
                caret.mode() == core::caret::mode::overwrite ? "OVR" : "INS"));
    }

    void document_footer::on_initialize() {
        _caret_status.initialize();
        _caret_status.palette(palette());
        _caret_status.fg_color(fg_color());
        _caret_status.bg_color(bg_color());
        _caret_status.dock(dock::styles::left);
        _caret_status.font_family(font_family());
        _caret_status.margin({0, font_face()->width, 0, 0 });

        _document_status.initialize();
        _document_status.palette(palette());
        _document_status.fg_color(fg_color());
        _document_status.bg_color(bg_color());
        _document_status.dock(dock::styles::left);
        _document_status.value("| project: (none)");
        _document_status.font_family(font_family());
        _document_status.margin({0, font_face()->width, 0, 0 });

        add_child(&_document_status);
        add_child(&_caret_status);

        bounds().height(font_face()->line_height);
    }

    void document_footer::on_resize(const core::rect& context_bounds) {
        dock_layout_panel::on_resize(context_bounds);
    }

}