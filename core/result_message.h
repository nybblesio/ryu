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
#include "core_types.h"

namespace ryu::core {

    class result_message {
    public:
        enum types {
            info,
            error,
            data
        };

        result_message(
                const std::string& code,
                const core::parameter_dict& params) : _type(types::data),
                                                      _code(code),
                                                      _params(params) {
        }

        result_message(
                const std::string& code,
                const std::string& message,
                const std::string& details = "",
                types type = types::info) : _type(type),
                                            _code(code),
                                            _message(message),
                                            _details(details) {
        }

        inline types type() const {
            return _type;
        }

        inline bool is_error() const {
            return _type == types::error;
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

        template <typename T>
        T get_parameter(const std::string& name) const {
            auto it = _params.find(name);
            if (it == _params.end())
                return {};
            return boost::get<T>(it->second);
        }

        inline const core::parameter_dict& params() const {
            return _params;
        }

    private:
        types _type;
        std::string _code;
        std::string _message {};
        std::string _details {};
        core::parameter_dict _params {};
    };

    typedef std::vector <result_message> result_message_list;

};

