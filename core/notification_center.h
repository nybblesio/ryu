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

#include <map>
#include <vector>
#include <string>
#include <functional>

//                  conceptual picture
//
// +=============+               +=============+
// | observer    |  -----------> | observable  |==+
// +=============+               +=============+  |
//      ^                           +=============+
//      |                                |
//      |                                |
//      |                                *
//      |                          some event happens
//      +--------------callback--------- *
//


//                  concrete picture
//
// +=============+                       +=================+
// | observer    |                       |    observable   |
// +=============+                       +=================+
//  void on_watch_notification(observable* ....)
//   ^   |                                        | const uint32_t id() const;
//   |   |                                        | const type_id type_id() const;
//   |   |                                        |
//   |   | watch an observable                    | register (id)
//   |   |   for a specific predicate             |
//   |   |                                        |
//   |   |            +=====================+     |
//   |   +----------> | notification_center | <---+
//   |                +=====================+
//   |                          |
//   |                          | notify_watchers
//   +====== callback ==========+
//

namespace ryu::core {

    class notification_center;
    using observable_type = uint32_t;

    class observable {
    public:
        virtual ~observable() = default;

        virtual uint32_t id() const = 0;

        virtual observable_type type_id() const = 0;

    protected:
        virtual void notify() = 0;
    };

    class observer {
    public:
        virtual ~observer() = default;

    protected:
        friend class notification_center;

        virtual void on_notification(observable* instance) = 0;

        virtual bool on_notification_filter(observable* instance) = 0;
    };

    class notification_center {
    public:
        static notification_center* instance();

        void clear_observers();

        void clear_observables();

        bool notify(observable* instance);

        void remove_observer(uint32_t id);

        void add_observable(observable* instance);

        void remove_observable(observable* instance);

        void add_observer(uint32_t id, observer* instance);

    private:
        std::map<uint32_t, observer*> _observers {};
        std::map<uint32_t, observable*> _observables {};
    };

};

