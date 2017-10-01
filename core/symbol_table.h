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

#include "result.h"
#include "core_types.h"

namespace ryu::core {

    class symbol_table {
    public:
        symbol_table() = default;

        bool missing_is_error() const {
            return _missing_is_error;
        }

        void missing_is_error(bool flag) {
            _missing_is_error = flag;
        }

        void remove(const std::string& name);

        std::vector<std::string> identifiers() {
            std::vector<std::string> identifiers;
            for (auto& symbol : _symbols) {
                identifiers.push_back(symbol.first);
            }
            return identifiers;
        }

        ast_node_shared_ptr get(const std::string& name) const;

        void put(const std::string& name, const ast_node_shared_ptr& value);

    private:
        symbol_dict _symbols;
        bool _missing_is_error = true;
    };

};

