//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <SDL2/SDL_log.h>
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
    }

    ///////////////////////////////////////////////////////////////////////////

    logger::logger(
            const std::string& name,
            logger::level::types level) : _level(level),
                                          _name(name) {
    }

    void logger::info(const std::string& msg) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str());
    }

    void logger::warn(const std::string& msg) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str());
    }

    void logger::error(const std::string& msg) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str());
    }

    void logger::fatal(const std::string& msg) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str());
    }

    void logger::result(const core::result& result) {
        for (auto& message : result.messages()) {
            if (message.is_error()) {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "%s",
                    fmt::format("{}: {}", message.code(), message.message()).c_str());
                if (!message.details().empty())
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.details().c_str());
            } else {
                SDL_LogWarn(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "%s",
                    fmt::format("{}: {}", message.code(), message.message()).c_str());
                if (!message.details().empty())
                    SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message.details().c_str());
            }
        }
    }

}
