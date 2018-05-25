//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <logger_factory.h>
#include "input_action_provider.h"

namespace ryu::core {

    static logger* s_log = logger_factory::instance()->create(
            "input_action_provider",
            logger::level::info);

    input_action_provider::input_action_provider() {
    }

    input_action_provider::~input_action_provider() {
    }

    bool input_action_provider::process_action(
            const input_action* action,
            const event_data_t& data) const {
        if (action == nullptr) {
            s_log->warn(fmt::format("::process_action: null input_action"));
            return false;
        }
        auto it = _handlers.find(action->id());
        if (it == _handlers.end())
            return false;
        auto result = it->second(data);
        //s_log->info(fmt::format("processed: action = {}, result = {}", action->name(), result));
        return result;
    }

    void input_action_provider::register_handler(
            const input_action* action,
            const input_action_perform& perform) {
        if (action == nullptr) {
            s_log->warn(fmt::format("::register_handler: null input_action"));
            return;
        }
        _handlers.insert(std::make_pair(action->id(), perform));
    }

    bool input_action_provider::process(pending_event_list& events) {
        if (_handlers.empty() || events.empty())
            return false;

        auto result = false;
        auto it = events.begin();
        while (it != events.end()) {
            auto event = *it;
            //s_log->info(fmt::format("pending_event_t: action->name() = {}", event.action->name()));
            bool processed = process_action(event.action, event.data);
            if (processed) {
                it = events.erase(it);
                result = true;
                if (!event.action->should_bubble())
                    break;
                s_log->info(fmt::format("pending_event_t: action->name() = {}, bubble", event.action->name()));
            } else {
                ++it;
            }
        }

        return result;
    }

}