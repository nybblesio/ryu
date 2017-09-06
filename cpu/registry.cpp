//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include "registry.h"

namespace ryu::cpu {

    registry::registry() {
        _models = {
                {models::ids::mc6809, "MC6809", "Motorola 6809 8-bit CPU", "Motorola", 65536}
        };

        _factories = {
                {models::ids::mc6809, [](int id, const std::string& name) {
                    return new mc6809::cpu(id, name);
                }}
        };
    }

    hardware::cpu* registry::create(
            models::ids model,
            int id,
            const std::string& name) {
        auto it = _factories.find(model);
        if (it == _factories.end())
            return nullptr;
        return it->second(id, name);
    }

    const model_list& registry::models() const {
        return _models;
    }

}