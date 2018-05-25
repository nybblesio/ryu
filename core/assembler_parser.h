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

    // *done: ?               - uninitialized
    // *done: $               - location counter alias
    // *done: .binary [path]  - include binary data into assembly
    // *done: .include [path] - recursively assembly file
    //
    // *done: dup             - .byte 5 dup $c0 or .byte 10 dup 2, 8
    //
    // *done: #               - paste operator
    // *done: `               - quote operator
    //
    // STILL TODO:
    // .loop                  - repeat the basic_block
    // .local                 - create a unique label name
    //
    // 1. directives
    // ---------------------------------
    // .org [addr] - changes the insertion point address *done
    // .target "name-of-component" *done
    // .equ *done
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
    // .struct [name]               *done
    //      label   .byte
    // .end
    //
    // .align [size]                *done
    //
    // .byte $00 | ?, $af, 256      *done except ? uninitialized
    //
    // string examples:
    //  .ascii "this is a string"    *done
    //  .byte "this is a string too" *done
    //
    // .word $ffff | ?, $a000, $c000 *done
    // .dword $ffffffff | ?          *done
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
    // examples from fasm:
    //
    //  # - paste operator/concatenate operator
    //  ` - converts any symbol into a string
    //
    // directives within macros/structures:
    //  common, forward, reverse
    //
    // $ = current location counter

    // 3. variables & labels
    // ----------------------------------
    //
    // [0000] foo  .byte   ?  * done except uninitialized variable
    //
    // Example:
    //      lda     #foo
    //
    // foo:                   * label (pointer)
    //      lda     #$0f
    //
    // [0100] foo: .word   ?  * done except unitialized, a label (pointer) to a word
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
    // .if [expression]         * done
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
    // .macro [name](param1, param2, ...) *done
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
        enum operators : uint16_t {
            dup = 500,
            quote,
            paste,
            offset
        };

        assembler_parser() = default;

        void register_operators();

        ast_node_shared_ptr parse(const parser_input_t& input) override;

    protected:
        ast_node_shared_ptr parse_directive();

        ast_node_shared_ptr parse_dup_operator();

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

