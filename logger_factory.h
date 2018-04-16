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

#include <any>
#include <string>
#include <vector>

namespace ryu {

    class logger {
    public:
        struct level {
            enum types {
                emergency,
                fatal,
                alert,
                critical,
                error,
                warn,
                notice,
                info,
                debug
            };
        };

        void info(const std::string& msg);

        void warn(const std::string& msg);

        void error(const std::string& msg);

        void fatal(const std::string& msg);

    private:
        friend class logger_factory;

        logger(
                const std::string& name,
                logger::level::types level);

    private:
        level::types _level;
        std::string _name;
        std::any _category;
    };

    class logger_factory {
    public:
        static logger_factory* instance();

        void initialize(const std::string& path);

        logger* create(
            const std::string& name,
            logger::level::types level);

    private:
        std::vector<logger> _loggers {};
    };

};