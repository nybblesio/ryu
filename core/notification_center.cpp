//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "notification_center.h"

namespace ryu::core {

    notification_center* notification_center::instance() {
        static notification_center instance {};
        return &instance;
    }

    void notification_center::clear_observers() {
        _observers.clear();
    }

    void notification_center::clear_observables() {
        _observables.clear();
    }

    void notification_center::remove_observer(uint32_t id) {
        _observers.erase(id);
    }

    bool notification_center::notify(observable* instance) {
        auto handled = false;
        for (const auto& it : _observers) {
            auto observer = it.second;
            if (observer == nullptr)
                continue;
            if (observer->on_notification_filter(instance)) {
                handled = true;
                observer->on_notification(instance);
            }
        }
        return handled;
    }

    void notification_center::add_observer(uint32_t id, observer* instance) {
        _observers.insert(std::make_pair(id, instance));
    }

    void notification_center::add_observable(observable* instance) {
        _observables.insert(std::make_pair(instance->id(), instance));
    }

    void notification_center::remove_observable(observable* instance) {
        _observables.erase(instance->id());
    }

}