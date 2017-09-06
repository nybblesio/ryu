//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <string>
#include <utility>
#include <vector>
#include "result_message.h"

namespace ryu::core {

    class result {
    public:
        result() : _success(true), _messages({}) {
        }

        inline void fail() {
            _success = false;
        }

        inline void succeed() {
            _success = true;
        }

        inline void add_message(
                const std::string& code,
                const std::string& message) {
            _messages.emplace_back(code, message, "", false);
        }

        inline void add_message(
                const std::string& code,
                const std::string& message,
                bool error) {
            _messages.emplace_back(code, message, "", error);
        }

        inline void add_message(
                const std::string& code,
                const std::string& message,
                const std::string& details,
                bool error) {
            _messages.emplace_back(code, message, details, error);
        }

        inline bool has_code(const std::string& code) const {
            for (const auto& msg : _messages)
                if (msg.code() == code)
                    return true;
            return false;
        }

        inline bool is_failed() const {
            return !_success;
        }

        inline const result_message_list & messages() const {
            return _messages;
        }

    private:
        bool _success = false;
        result_message_list _messages;
    };

};
