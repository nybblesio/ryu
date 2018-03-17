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

#include <log4cpp/Category.hh>

namespace ryu {

    class logger {
    public:
        static logger* instance();

        log4cpp::Category* category(
            const std::string& name,
            log4cpp::Priority::PriorityLevel priority);

        log4cpp::Category* root() const;
    };

};

