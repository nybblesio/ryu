//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "input_action_provider.h"

namespace ryu::core {

    input_action_provider::input_action_provider() {
    }

    input_action_provider::~input_action_provider() {
    }

    //    void input_action::register_handler(
//            action_sink_type type,
//            const input_action_filter& filter,
//            const input_action_perform& perform) {
//        _handlers.insert(std::make_pair(
//            type,
//            input_action_handler_t {filter, perform}));
//    }

    //    action_sink_type input_action::process_action(
//            const input_binding& binding,
//            const event_data_t& data) const {
//        for (uint16_t i = action_sink::last; i > action_sink::none; i--) {
//            auto it = _handlers.find(i);
//            if (it == _handlers.end())
//                continue;
//            if (!it->second.filter(data))
//                continue;
//            if (it->second.perform(data))
//                return (action_sink_type) i;
//        }
//        return action_sink::none;
//    }

}