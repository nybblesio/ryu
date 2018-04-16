//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include "logger_factory.h"

namespace ryu {

    logger_factory* logger_factory::instance() {
        static logger_factory s_instance;
        if (s_instance._loggers.capacity() < 256)
            s_instance._loggers.reserve(256);
        return &s_instance;
    }

    logger* logger_factory::create(
            const std::string& name,
            logger::level::types level) {
        logger instance(name, level);
        _loggers.push_back(instance);
        return &_loggers.back();
    }

    void logger_factory::initialize(const std::string& path) {
        log4cpp::PropertyConfigurator::configure(path);
    }

    ///////////////////////////////////////////////////////////////////////////

    logger::logger(
            const std::string& name,
            logger::level::types level) : _level(level),
                                          _name(name) {
        auto category = &log4cpp::Category::getInstance(name);
        _category = category;
        switch (_level) {
            case level::emergency:
                category->setPriority(log4cpp::Priority::EMERG);
                break;
            case level::fatal:
                category->setPriority(log4cpp::Priority::FATAL);
                break;
            case level::alert:
                category->setPriority(log4cpp::Priority::ALERT);
                break;
            case level::critical:
                category->setPriority(log4cpp::Priority::CRIT);
                break;
            case level::error:
                category->setPriority(log4cpp::Priority::ERROR);
                break;
            case level::warn:
                category->setPriority(log4cpp::Priority::WARN);
                break;
            case level::notice:
                category->setPriority(log4cpp::Priority::NOTICE);
                break;
            case level::info:
                category->setPriority(log4cpp::Priority::INFO);
                break;
            case level::debug:
                category->setPriority(log4cpp::Priority::DEBUG);
                break;
        }
    }

    void logger::info(const std::string& msg) {
        auto category = std::any_cast<log4cpp::Category*>(_category);
        category->info(msg);
    }

    void logger::warn(const std::string& msg) {
        auto category = std::any_cast<log4cpp::Category*>(_category);
        category->warn(msg);
    }

    void logger::error(const std::string& msg) {
        auto category = std::any_cast<log4cpp::Category*>(_category);
        category->error(msg);
    }

    void logger::fatal(const std::string& msg) {
        auto category = std::any_cast<log4cpp::Category*>(_category);
        category->fatal(msg);
    }

}
