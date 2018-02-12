//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//
// See the LICENSE file for details about the license covering
// this source code file.
//

#define CATCH_CONFIG_RUNNER
#include <catch2.hpp>

int main(int argc, char** argv) {
    auto result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}