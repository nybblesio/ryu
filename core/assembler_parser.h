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

#include "parser.h"

namespace ryu::core {

    // 0. possibilities
    // ---------------------------------
    //
    // examples from fasm:
    //
    //  # - paste operator/concatenate operator
    //  ` - converts any symbol into a string
    //
    // directives within macros/structures:
    //  common, forward, reverse
    //
    //
    // ---------------------------------
    //
    //
    // 1. directives
    // ---------------------------------
    // .org [addr] - changes the insertion point address
    // .target "name-of-component"
    // .equ | =
    //
    // this includes assembly text:
    // .include [path]
    //
    // this includes binary data
    // .binary [path]
    //
    //
    // data directives
    // ---------------------------------
    //
    // .struct [name]
    //      label   .byte
    // .end
    //
    // .align [size]
    //
    // .byte $00 | ?, $af, 256
    //
    // string examples:
    //  .ascii "this is a string"
    //  .byte "this is a string too"
    //
    // .word $ffff | ?, $a000, $c000
    // .dword $ffffffff | ?
    //

    // 2. operators & functions
    // ----------------------------------
    // dup [size]([initializer] = ? | number)
    //
    // push this down to cpu specific parser:
    // -    > and < are operators for nybbles
    // example:
    //      lda >#$ffaa
    //      lda <#$ffaa
    //
    // $ = current location counter

    // 3. variables & labels
    // ----------------------------------
    //
    // [0000] foo  .byte   ?  * uninitialized variable
    //
    // Example:
    //      lda     #foo
    //
    // foo:                   * label (pointer)
    //      lda     #$0f
    //
    // [0100] foo: .word   ?  * a label (pointer) to a word
    //
    // Example:
    //      ldd     #foo      * load effective address
    //
    // .local [name]          * scoped to the current block
    //
    //
    // for branches, what symbol to use for backward/forward?
    //
    // 4. macros
    // ----------------------------------
    //
    // .if [expression]         * has to evaluate to bool
    // .elseif [expression]
    // .else
    // .end
    //
    // .loop [identifier] [count] <[start]> <[step]>
    //                      ^^
    //                      ||=== these can be tokens passed into macro
    //
    //      .if DEBUG
    //          ldx #$0f
    //      .end
    //
    // .end
    //
    //
    // .macro [name](param1, param2, ...)
    //      .local silly_loop
    //
    //  silly_loop:
    //      bra silly_loop
    //
    // .end
    //
    // Usage Example:
    //
    //      ldy         #$ff
    //      repeat_char 'A'     * this expands the macro
    //
    // 5. cpu opcode/address mode parsing -> code generation
    //    this comes from cpu_specific plugin

    class assembler_parser : public core::parser {
    public:
        assembler_parser() = default;

        ast_node_shared_ptr parse(const std::string& input) override;

    protected: // directive related parsers
        ast_node_shared_ptr parse_directive();

    protected:
        void parse_assembly();

    private:
        ast_node_shared_ptr pop_scope();

        ast_node_shared_ptr current_scope() const;

        ast_node_shared_ptr push_scope(const ast_node_shared_ptr& node);

    private:
        std::stack<ast_node_shared_ptr> _scope_stack {};
    };

};

