//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#include <rttr/type>
#include <hardware/hardware.h>
#include "controller.h"

namespace ryu::ide::machine_editor {

    controller::controller(
            core::context* context,
            int id,
            const std::string& name) : core::state(context, id, name) {
    }

    void controller::on_update(uint32_t dt) {
    }

    void controller::on_init(SDL_Renderer* renderer) {
        auto base = rttr::type::get<ryu::hardware::integrated_circuit>();
        auto component_types = base.get_derived_classes();
        for(auto& t : component_types) {
            std::cout << t.get_name() << "\n";
            std::cout << "properties:\n";
            for (auto& p : t.get_properties())
                std::cout << "\tname: " << p.get_name() << "\n";
        }
    }

    void controller::on_draw(SDL_Renderer* renderer) {
    }

    bool controller::on_process_event(const SDL_Event* e) {
        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_ESCAPE: {
                    end_state();
                    return true;
                }
            }
        }
        return false;
    }

}