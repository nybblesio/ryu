//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

#pragma once

#include <map>
#include <stack>
#include <list>
#include <common/SDL_FontCache.h>

namespace ryu::core {

    class engine;

    class timer;

    typedef std::vector<timer*> timer_list;
    typedef std::function<void (timer*)> timer_callable;

    class context;

    typedef std::map<int, context*> context_dict;

    class state;

    typedef std::function<bool (state*, const std::string&)> state_transition_callable;

    struct state_t {
        state* instance;
    };

    typedef std::map<int, state_t> state_dict;

    typedef std::map<std::string, std::string> blackboard;

    typedef std::map<std::string, std::string> parameter_dict;

};