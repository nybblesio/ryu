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

namespace ryu::core {

    typedef std::vector<std::string> option_list;

    class pick_list : public core::view {
    public:
        pick_list(core::context* context, const std::string& name);

        int length() const;

        std::string value();

        option_list& options();

        void length(int value);

        border::types border() const;

        void border(border::types value);

        void value(const std::string& value);

    protected:
        void on_draw() override;

        bool on_process_event(const SDL_Event* e) override;

    private:
        int _length = 32;
        std::string _value;
        option_list _options {};
        border::types _border = border::types::none;
    };

};

