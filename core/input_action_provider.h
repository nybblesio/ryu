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

#include <functional>
#include "input_binding.h"
#include "input_action.h"

namespace ryu::core {

    using input_action_filter = std::function<bool (const event_data_t&)>;
    using input_action_perform = std::function<bool (const event_data_t&)>;

    struct input_action_handler_t {
        input_action_filter filter;
        input_action_perform perform;

        static bool default_filter(const event_data_t& data) {
            return true;
        }

        static bool default_perform(const event_data_t& data) {
            return true;
        }
    };

    class input_action_provider {
    public:
        input_action_provider();

        virtual ~input_action_provider();

    private:
        std::map<action_id, input_action_handler_t> _handlers {};
    };

};

