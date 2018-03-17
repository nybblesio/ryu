//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "logger.h"

namespace ryu {

    logger* logger::instance() {
        static logger s_instance;
        return &s_instance;
    }

    log4cpp::Category* logger::category(
            const std::string& name,
            log4cpp::Priority::PriorityLevel priority) {
        auto category = &log4cpp::Category::getInstance(name);
        category->setPriority(priority);
        return category;
    }

    log4cpp::Category* logger::root() const {
        return &log4cpp::Category::getRoot();
    }

}
