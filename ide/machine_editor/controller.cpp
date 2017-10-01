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
#include <core/engine.h>
#include <hardware/hardware.h>
#include <hardware/registry.h>
#include "controller.h"

namespace ryu::ide::machine_editor {

    controller::controller(const std::string& name) : core::state(name),
                                                      _view("editor") {
    }

    void controller::on_draw() {
        _view.draw(context()->renderer());
    }

    void controller::on_resize() {
        _view.resize(context()->bounds());
    }

    void controller::on_deactivate() {
        _machine = nullptr;
    }

    void controller::on_initialize() {
        _view.font_family(context()->engine()->find_font_family("hack"));
        _view.palette(context()->palette());
        _view.initialize();
        _view.focus(_view.id());

//        auto base = rttr::type::get<ryu::hardware::integrated_circuit>();
//        auto component_types = base.get_derived_classes();
//        for(auto& t : component_types) {
//            std::cout << t.get_name() << "\n";
//            std::cout << "properties:\n";
//            for (auto& p : t.get_properties())
//                std::cout << "\tname: " << p.get_name() << "\n";
//        }
    }

    void controller::on_update(uint32_t dt) {
    }

    hardware::machine* controller::machine() {
        return _machine;
    }

    void controller::machine(hardware::machine* value) {
        _view.machine(value);
        _machine = value;
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
        return _view.process_event(e);
    }

    void controller::on_activate(const core::parameter_dict& params) {
        auto it = params.find("name");
        if (it != params.end()) {
            auto mach = hardware::registry::instance()->find_machine(it->second);
            if (mach == nullptr) {
                mach = hardware::registry::instance()->new_machine();
                mach->name(it->second);
            }
            machine(mach);
        }
    }

}