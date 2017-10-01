//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "symbol_table.h"

namespace ryu::core {

    void symbol_table::remove(const std::string& name) {
        _symbols.erase(name);
    }

    ast_node_shared_ptr symbol_table::get(const std::string& name) const {
        auto it = _symbols.find(name);
        if (it == _symbols.end())
            return nullptr;
        return it->second;
    }

    void symbol_table::put(const std::string& name, const ast_node_shared_ptr& value) {
        _symbols[name] = value;
    }

}