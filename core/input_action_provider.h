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

    using input_action_perform = std::function<bool (const event_data_t&)>;

    class input_action_provider {
    public:
        input_action_provider();

        virtual ~input_action_provider();

        void register_handler(
                const input_action* action,
                const input_action_perform& perform);

        bool process(pending_event_list& events);

    private:
        bool process_action(
                const input_action* action,
                const event_data_t& data) const;

    private:
        std::map<action_id, input_action_perform> _handlers {};
    };

};

