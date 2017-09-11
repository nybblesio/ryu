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

#include <vector>
#include <string>

namespace ryu::core {

    class result_message {
    public:
        result_message(
                std::string code,
                std::string message,
                std::string details = "",
                bool error = false) : _error(error),
                                      _code(std::move(code)),
                                      _message(std::move(message)),
                                      _details(std::move(details)) {
        }

        inline bool is_error() const {
            return _error;
        }

        inline const std::string& code() const {
            return _code;
        }

        inline const std::string& details() const {
            return _details;
        }

        inline const std::string& message() const {
            return _message;
        }

    private:
        bool _error;
        std::string _code;
        std::string _message;
        std::string _details;
    };

    typedef std::vector <result_message> result_message_list;

};

