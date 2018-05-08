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

#include <core/view.h>
#include <logger_factory.h>

namespace ryu::core {

    class label : public core::view {
    public:
        label(
            const std::string& name,
            core::view_host* host);

        ~label() override;

        std::string value() const override;

        void value(const std::string& text) override;

        core::alignment::vertical::types valign() const;

        core::alignment::horizontal::types halign() const;

        void valign(core::alignment::vertical::types value);

        void halign(core::alignment::horizontal::types value);

    protected:
        void on_draw(ryu::core::renderer& surface) override;

    private:
        void update_minimum_size();

    private:
        core::alignment::vertical::types _valign = core::alignment::vertical::middle;
        core::alignment::horizontal::types _halign = core::alignment::horizontal::none;
    };

};

