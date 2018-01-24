//
// Ryu
//
// Copyright (C) 2017 Jeff Panici
// All Rights Reserved.
//

/**
----------------------------------------------------------------
|                                                              |
|                                                              |
|                           Motorola                           |
|                                                              |
|              666      88888      000      99999              |
|             6        8     8    0   0    9     9             |
|            6         8     8   0   0 0   9     9             |
|            666666     88888    0  0  0    999999             |
|            6     6   8     8   0 0   0         9             |
|            6     6   8     8    0   0         9              |
|             66666     88888      000      9999               |
|                                                              |
|         6809 MICROPROCESSOR Instruction Set Summary          |
|                                                              |
|                                                              |
|                                                              |
|                                                              |
|                                                              |
|                    _________    _________                    |
|                  _|         \__/         |_  ____            |
|             Vss |_|1                   40|_| Halt <--        |
|             ___  _|                      |_                  |
|         --> NMI |_|2                   39|_| XTAL <--        |
|             ___  _|                      |_                  |
|         --> IRQ |_|3                   38|_| EXTAL <--       |
|            ____  _|                      |_  _____           |
|        --> FIRQ |_|4                   37|_| Reset <--       |
|                  _|                      |_                  |
|          <-- BS |_|5                   36|_| MRDY <--        |
|                  _|                      |_                  |
|          <-- BA |_|6                   35|_| Q -->           |
|                  _|                      |_                  |
|             Vcc |_|7                   34|_| E -->           |
|                  _|                      |_  ___ ____        |
|          <-- A0 |_|8                   33|_| DMA/BREQ <--    |
|                  _|                      |_    _             |
|          <-- A1 |_|9                   32|_| R/W -->         |
|                  _|                      |_                  |
|          <-- A2 |_|10       6809       31|_| D0 <-->         |
|                  _|                      |_                  |
|          <-- A3 |_|11                  30|_| D1 <-->         |
|                  _|                      |_                  |
|          <-- A4 |_|12                  29|_| D2 <-->         |
|                  _|                      |_                  |
|          <-- A5 |_|13                  28|_| D3 <-->         |
|                  _|                      |_                  |
|          <-- A6 |_|14                  27|_| D4 <-->         |
|                  _|                      |_                  |
|          <-- A7 |_|15                  26|_| D5 <-->         |
|                  _|                      |_                  |
|          <-- A8 |_|16                  25|_| D6 <-->         |
|                  _|                      |_                  |
|          <-- A9 |_|17                  24|_| D7 <-->         |
|                  _|                      |_                  |
|         <-- A10 |_|18                  23|_| A15 -->         |
|                  _|                      |_                  |
|         <-- A11 |_|19                  22|_| A14 -->         |
|                  _|                      |_                  |
|         <-- A12 |_|20                  21|_| A13 -->         |
|                   |______________________|                   |
|                                                              |
|                                                              |
|                                                              |
|                                                              |
|                                                              |
|                                                              |
|Written by     Jonathan Bowen                                 |
|               Programming Research Group                     |
|               Oxford University Computing Laboratory         |
|               8-11 Keble Road                                |
|               Oxford OX1 3QD                                 |
|               England                                        |
|                                                              |
|               Tel +44-865-273840                             |
|                                                              |
|Created        August 1981                                    |
|Updated        April 1985                                     |
|Issue          1.5                Copyright (C) J.P.Bowen 1985|
----------------------------------------------------------------
----------------------------------------------------------------
|Mnemon.|Op|IHNZVC|IEXD#R|~|Description           |Notes       |
|-------+--+------+------+-+----------------------+------------|
|ABX    |3A|------|X     |3|Add to Index Register |X=X+B       |
|ADCa  s|B9|-*****| XXXXX|5|Add with Carry        |a=a+s+C     |
|ADDa  s|BB|-*****| XXXXX|5|Add                   |a=a+s       |
|ADDD  s|F3|-*****| XXX*X|7|Add to Double acc.    |D=D+s       |
|ANDa  s|B4|--**0-| XXXXX|5|Logical AND           |a=a&s       |
|ANDCC s|1C|?????1|    X |3|Logical AND with CCR  |CC=CC&s     |
|ASL   d|78|--****| XXX X|7|Arithmetic Shift Left |d=d*2       |
|ASLa   |48|--****|X     |2|Arithmetic Shift Left |a=a*2       |
|ASR   d|77|--****| XXX X|7|Arithmetic Shift Right|d=d/2       |
|ASRa   |47|--****|X     |2|Arithmetic Shift Right|a=a/2       |
|BCC   m|24|------|     x|3|Branch if Carry Clear |If C=0      |
|BCS   m|25|------|     x|3|Branch if Carry Set   |If C=1      |
|BEQ   m|27|------|     x|3|Branch if Equal       |If Z=1      |
|BGE   m|2C|------|     x|3|Branch if Great/Equal |If NxV=0    |
|BGT   m|2E|------|     x|3|Branch if Greater Than|If Zv{NxV}=0|
|BHI   m|22|------|     x|3|Branch if Higher      |If CvZ=0    |
|BHS   m|24|------|     x|3|Branch if Higher/Same |If C=0      |
|BITa  s|B5|--**0-| XXXXX|5|Bit Test accumulator  |a&s         |
|BLE   m|2F|------|     x|3|Branch if Less/Equal  |If Zv{NxV}=1|
|BLO   m|25|------|     x|3|Branch if Lower       |If C=1      |
|BLS   m|23|------|     x|3|Branch if Lower/Same  |If CvZ=1    |
|BLT   m|2D|------|     x|3|Branch if Less Than   |If NxV=1    |
|BMI   m|2B|------|     x|3|Branch if Minus       |If N=1      |
|BNE   m|26|------|     x|3|Branch if Not Equal   |If Z=0      |
|BPL   m|2A|------|     x|3|Branch if Plus        |If N=0      |
|BRA   m|20|------|     x|3|Branch Always         |PC=m        |
|BRN   m|21|------|     x|3|Branch Never          |NOP         |
|BSR   m|8D|------|     x|7|Branch to Subroutine  |-[S]=PC,BRA |
|BVC   m|28|------|     x|3|Branch if Overflow Clr|If V=0      |
|BVS   m|29|------|     x|3|Branch if Overflow Set|If V=1      |
|CLR   d|7F|--0100| XXX X|7|Clear                 |d=0         |
|CLRa   |4F|--0100|X     |2|Clear accumulator     |a=0         |
|CMPa  s|B1|--****| XXXXX|5|Compare               |a-s         |
|CMPD  s|B3|--****| XXX*X|8|Compare Double acc.   |D-s    (10H)|
|CMPS  s|BC|--****| XXX*X|8|Compare Stack pointer |S-s    (11H)|
|CMPU  s|B3|--****| XXX*X|8|Compare User stack ptr|U-s    (11H)|
|CMPi  s|BC|--****| XXX*X|7|Compare               |i-s (Y ~s=8)|
|COM   d|73|--**01| XXX X|2|Complement            |d=~d        |
|COMa   |43|--**01|X     |7|Complement accumulator|a=~a        |
|CWAI  n|3C|E?????|    X |K|AND CCR, Wait for int.|CC=CC&n,E=1,|
|DAA    |19|--****|X     |2|Decimal Adjust Acc.   |A=BCD format|
|DEC   d|7A|--***-| XXX X|7|Decrement             |d=d-1       |
|DECa   |4A|--***-|X     |2|Decrement accumulator |a=a-1       |
|EORa  s|B8|--**0-| XXXXX|5|Logical Exclusive OR  |a=axs       |
|EXG r,r|1E|------|X     |8|Exchange (r1 size=r2) |r1<->r2     |
|INC   d|7C|--***-| XXX X|7|Increment             |d=d+1       |
|INCa   |4C|--***-|X     |2|Increment accumulator |a=a+1       |
|JMP   s|7E|------| XXX X|4|Jump                  |PC=EAs      |
|JSR   s|BD|------| XXX X|8|Jump to Subroutine    |-[S]=PC,JMP |
|LBcc nn|10|------|     x|5|Long cond. Branch(~=6)|If cc LBRA  |
|LBRA nn|16|------|     x|5|Long Branch Always    |PC=nn       |
|LBSR nn|17|------|     x|9|Long Branch Subroutine|-[S]=PC,LBRA|
|LDa   s|B6|--**0-| XXXXX|5|Load accumulator      |a=s         |
|LDD   s|FC|--**0-| XXX*X|6|Load Double acc.      |D=s         |
|LDS   s|FE|--**0-| XXX*X|7|Load Stack pointer    |S=s    (10H)|
|LDU   s|FE|--**0-| XXX*X|6|Load User stack ptr   |U=s         |
|LDi   s|BE|--**0-| XXX*X|6|Load index register   |i=s (Y ~s=7)|
|LEAp  s|3X|---i--| xX  X|4|Load Effective Address|p=EAs(X=0-3)|
|LSL   d|78|--0***| XXX X|7|Logical Shift Left    |d={C,d,0}<- |
|LSLa   |48|--0***|X     |2|Logical Shift Left    |a={C,a,0}<- |
|LSR   d|74|--0***| XXX X|7|Logical Shift Right   |d=->{C,d,0} |
|LSRa   |44|--0***|X     |2|Logical Shift Right   |d=->{C,d,0} |
|MUL    |3D|---*-*|X     |B|Multiply              |D=A*B       |
|NEG   d|70|-?****| XXX X|7|Negate                |d=-d        |
|NEGa   |40|-?****|X     |2|Negate accumulator    |a=-a        |
|NOP    |12|------|X     |2|No Operation          |            |
|ORa   s|BA|--**0-| XXXXX|5|Logical inclusive OR  |a=avs       |
|ORCC  n|1A|??????|    X |3|Inclusive OR CCR      |CC=CCvn     |
|PSHS  r|34|------|X     |2|Push reg(s) (not S)   |-[S]={r,...}|
|PSHU  r|36|------|X     |2|Push reg(s) (not U)   |-[U]={r,...}|
|PULS  r|35|??????|X     |2|Pull reg(s) (not S)   |{r,...}=[S]+|
|PULU  r|37|??????|X     |2|Pull reg(s) (not U)   |{r,...}=[U]+|
|ROL   d|79|--****| XXX X|7|Rotate Left           |d={C,d}<-   |
|ROLa   |49|--****|X     |2|Rotate Left acc.      |a={C,a}<-   |
|ROR   d|76|--****| XXX X|7|Rotate Right          |d=->{C,d}   |
|RORa   |46|--****|X     |2|Rotate Right acc.     |a=->{C,a}   |
----------------------------------------------------------------
----------------------------------------------------------------
|Mnemon.|Op|IHNZVC|IEXD#R|~|Description           |Notes       |
|-------+--+------+------+-+----------------------+------------|
|RTI    |3B|-*****|X     |6|Return from Interrupt |{regs}=[S]+ |
|RTS    |39|------|X     |5|Return from Subroutine|PC=[S]+     |
|SBCa  s|B2|--****| XXXXX|5|Subtract with Carry   |a=a-s-C     |
|SEX    |1D|--**--|X     |2|Sign Extend           |D=B         |
|STa   d|B7|--**0-| XXX X|5|Store accumultor      |d=a         |
|STD   d|FD|--**0-| XXX X|6|Store Double acc.     |D=a         |
|STS   d|FF|--**0-| XXX X|7|Store Stack pointer   |S=a    (10H)|
|STU   d|FF|--**0-| XXX X|6|Store User stack ptr  |U=a         |
|STi   d|BF|--**0-| XXX X|6|Store index register  |i=a (Y ~s=7)|
|SUBa  s|B0|--****| XXXXX|5|Subtract              |a=a-s       |
|SUBD  s|B3|--****| XXX*X|7|Subtract Double acc.  |D=D-s       |
|SWI    |3F|1-----|X     |J|Software Interrupt 1  |-[S]={regs} |
|SWI2   |3F|E-----|X     |K|Software Interrupt 2  |SWI    (10H)|
|SWI3   |3F|E-----|X     |K|Software Interrupt 3  |SWI    (11H)|
|SYNC   |13|------|X     |2|Sync. to interrupt    |  (min ~s=2)|
|TFR r,r|1F|------|X     |6|Transfer (r1 size<=r2)|r2=r1       |
|TST   s|7D|--**0-| XXX X|7|Test                  |s           |
|TSTa   |4D|--**0-|X     |2|Test accumulator      |a           |
|----------+------+------+-+-----------------------------------|
| CCR      |-*01? |      | |Unaffect/affected/reset/set/unknown|
| E        |E     |      | |Entire flag (Bit 7, if set RTI~s=F)|
| F  I     |I     |      | |FIRQ/IRQ interrupt mask (Bit 6/4)  |
| H        | H    |      | |Half carry (Bit 5)                 |
| N        |  N   |      | |Negative (Bit 3)                   |
| Z        |   Z  |      | |Zero (Bit 2)                       |
| V        |    V |      | |Overflow (Bit 1)                   |
| C        |     C|      | |Carry/borrow (Bit 0)               |
|-----------------+------+-------------------------------------|
| a               |I     | |Inherent (a=A,Op=4XH, a=B,Op=5XH)  |
| nn,E            | E    | |Extended (Op=E, ~s=e)              |
| [nn]            | x    | |Extended indirect                  |
| xx,p!           |  X   | |Indexed (Op=E-10H, ~s=e-1)         |
| [xx,p!]         |  X   | |Indexed indirect (p!=p++,--p only) |
| n,D             |   D  | |Direct (Op=E-20H, ~s=e-1)          |
| #n              |    # | |Immediate (8-bit, Op=E-30H, ~s=e-3)|
| #nn             |    * | |Immediate (16-bit)                 |
| m               |     x| |Relative (PC=PC+2+offset)          |
| [m]             |     R| |Relative indirect (ditto)          |
|--------------------------+-----------------------------------|
|DIRECT                    |Direct addressing mode             |
|EXTEND                    |Extended addressing mode           |
|FCB      n                |Form Constant Byte                 |
|FCC      'string'         |Form Constant Characters           |
|FDB      nn               |Form Double Byte                   |
|RMB      nn               |Reserve Memory Bytes               |
|--------------------------+-----------------------------------|
| A  B                     |Accumulators (8-bit)               |
| CC                       |Condition Code register (8-bit)    |
| D                        |A and B (16-bit, A high, B low)    |
| DP                       |Direct Page register (8-bit)       |
| PC                       |Program Counter (16-bit)           |
| S  U                     |System/User stack pointer(16-bit)  |
| X  Y                     |Index registers (16-bit)           |
|--------------------------+-----------------------------------|
| a                        |Acc A or B (a=A,Op=BXH, a=B,Op=FXH)|
| d  s  EA                 |Destination/source/effective addr. |
| i  p  r                  |Regs X,Y/regs X,Y,S,U/any register |
| m                        |Relative address (-126 to +129)    |
| n  nn                    |8/16-bit expression(0 to 255/65535)|
| xx  p!                   |A,B,D,nn/p+,-p,p++,--p (indexed)   |
| +  -  *  /               |Add/subtract/multiply/divide       |
| &  ~  v  x               |AND/NOT/inclusive OR/exclusive OR  |
| <-  ->  <->              |Rotate left/rotate right/exchange  |
| [ ]  [ ]+  -[ ]          |Indirect address/increment/decr.   |
| { }                      |Combination of operands            |
| {regs}                   |If E {PC,U/S,Y,X,DP,B,A,CC}/{PC,CC}|
| (10H)  (11H)             |Hex opcode to precede main opcode  |
|--------------------------+-----------------------------------|
| FFF0H to FFF1H           |Reserved by Motorola               |
| FFF2H to FFF3H           |SWI3 instruction interrupt vector  |
| FFF4H to FFF5H           |SWI2 instruction interrupt vector  |
| FFF6H to FFF7H           |Fast hardware int. vector (FIRQ)   |
| FFF8H to FFF9H           |Hardware interrupt vector (IRQ)    |
| FFFAH to FFFBH           |SWI instruction interrupt vector   |
| FFFCH to FFFDH           |Non-maskable interrupt vector (NMI)|
| FFFEH to FFFFH           |Reset vector                       |
----------------------------------------------------------------
*/

#include "cpu.h"

RTTR_REGISTRATION {
    rttr::registration::class_<ryu::hardware::mc6809::cpu>("ryu::hardware::mc6809::cpu") (
            rttr::metadata(ryu::hardware::meta_data_key::type_id, ryu::hardware::cpu_id),
            rttr::metadata(ryu::hardware::meta_data_key::type_name, "Motorola 6809 CPU IC")
        )
        .constructor<>(rttr::registration::public_access) (
                rttr::policy::ctor::as_raw_ptr
        );
}

namespace ryu::hardware::mc6809 {

    mnemonic_set cpu::_mnemonics;
    opcode_dictionary cpu::_opcodes;

    const mnemonic_set& cpu::mnemonics() {
        return _mnemonics;
    }

    const opcode_dictionary& cpu::opcodes() {
        return _opcodes;
    }

    cpu::cpu() : hardware::integrated_circuit("mc6809-ic") {
    }

    core::assembly_language_parser* cpu::assembler() {
        return &_parser;
    }

    void cpu::init() {
        _opcodes[0x003a] = opcode(
                0x003a,
                3,
                opcode::address_modes::inherent,
                {"abx"});

        _opcodes[0x0089] = opcode(
                0x0089,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"adca"});

        _opcodes[0x00c9] = opcode(
                0x00c9,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"adcb"});

        _opcodes[0x008b] = opcode(
                0x008b,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"adda"});

        _opcodes[0x00cb] = opcode(
                0x00cb,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"addb"});

        _opcodes[0x00c3] = opcode(
                0x00c3,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"addd"});

        _opcodes[0x0084] = opcode(
                0x0084,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"anda"});

        _opcodes[0x00c4] = opcode(
                0x00c4,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"andb"});

        _opcodes[0x001c] = opcode(
                0x001c,
                3,
                opcode::address_modes::immediate,
                {"andcc"});

        _opcodes[0x0008] = opcode(
                0x0008,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"asl"});

        _opcodes[0x0048] = opcode(
                0x0048,
                2,
                opcode::address_modes::inherent,
                {"asla"});

        _opcodes[0x0058] = opcode(
                0x0058,
                2,
                opcode::address_modes::inherent,
                {"aslb"});

        _opcodes[0x0007] = opcode(
                0x0007,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"asr"});

        _opcodes[0x0047] = opcode(
                0x0047,
                2,
                opcode::address_modes::inherent,
                {"asra"});

        _opcodes[0x0057] = opcode(
                0x0057,
                2,
                opcode::address_modes::inherent,
                {"asrb"});

        _opcodes[0x0024] = opcode(
                0x0024,
                3,
                opcode::address_modes::relative,
                {"bcc", "bhs"});

        _opcodes[0x0025] = opcode(
                0x0025,
                3,
                opcode::address_modes::relative,
                {"bcs", "blo"});

        _opcodes[0x0027] = opcode(
                0x0027,
                3,
                opcode::address_modes::relative,
                {"beq"});

        _opcodes[0x2c] = opcode(
                0x002c,
                3,
                opcode::address_modes::relative,
                {"bge"});

        _opcodes[0x002e] = opcode(
                0x002e,
                3,
                opcode::address_modes::relative,
                {"bgt"});

        _opcodes[0x0022] = opcode(
                0x0022,
                3,
                opcode::address_modes::relative,
                {"bhi"});

        _opcodes[0x0085] = opcode(
                0x0085,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"bita"});

        _opcodes[0x00c5] = opcode(
                0x00c5,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"bitb"});

        _opcodes[0x002f] = opcode(
                0x002f,
                3,
                opcode::address_modes::relative,
                {"ble"});

        _opcodes[0x0023] = opcode(
                0x0023,
                3,
                opcode::address_modes::relative,
                {"bls"});

        _opcodes[0x0025] = opcode(
                0x0025,
                3,
                opcode::address_modes::relative,
                {"blo"});

        _opcodes[0x002d] = opcode(
                0x002d,
                3,
                opcode::address_modes::relative,
                {"blt"});

        _opcodes[0x002b] = opcode(
                0x002b,
                3,
                opcode::address_modes::relative,
                {"bmi"});

        _opcodes[0x0026] = opcode(
                0x0026,
                3,
                opcode::address_modes::relative,
                {"bne"});

        _opcodes[0x002a] = opcode(
                0x002a,
                3,
                opcode::address_modes::relative,
                {"bpl"});

        _opcodes[0x0020] = opcode(
                0x0020,
                3,
                opcode::address_modes::relative,
                {"bra"});

        _opcodes[0x0021] = opcode(
                0x0021,
                3,
                opcode::address_modes::relative,
                {"brn"});

        _opcodes[0x008d] = opcode(
                0x008d,
                7,
                opcode::address_modes::relative,
                {"bsr"});

        _opcodes[0x0028] = opcode(
                0x0028,
                3,
                opcode::address_modes::relative,
                {"bvc"});

        _opcodes[0x0029] = opcode(
                0x0029,
                3,
                opcode::address_modes::relative,
                {"bvs"});

        _opcodes[0x000f] = opcode(
                0x000f,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"clr"});

        _opcodes[0x004f] = opcode(
                0x004f,
                2,
                opcode::address_modes::inherent,
                {"clra"});

        _opcodes[0x005f] = opcode(
                0x005f,
                2,
                opcode::address_modes::inherent,
                {"clrb"});

        _opcodes[0x0081] = opcode(
                0x0081,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"cmpa"});

        _opcodes[0x00c1] = opcode(
                0x00c1,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"cmpb"});

        _opcodes[0x1083] = opcode(
                0x1083,
                8,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"cmpd"});

        _opcodes[0x11bc] = opcode(
                0x118c,
                8,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"cmps"});

        _opcodes[0x11b3] = opcode(
                0x1183,
                8,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"cmpu"});

        _opcodes[0x008c] = opcode(
                0x008c,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"cmpx"});

        _opcodes[0x108c] = opcode(
                0x108c,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"cmpy"});

        _opcodes[0x0003] = opcode(
                0x0003,
                2,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"com"});

        _opcodes[0x0043] = opcode(
                0x0043,
                2,
                opcode::address_modes::inherent,
                {"coma"});

        _opcodes[0x0053] = opcode(
                0x0053,
                2,
                opcode::address_modes::inherent,
                {"comb"});

        _opcodes[0x003c] = opcode(
                0x003c,
                2,
                opcode::address_modes::immediate,
                {"cwai"});

        _opcodes[0x0019] = opcode(
                0x0019,
                2,
                opcode::address_modes::inherent,
                {"daa"});

        _opcodes[0x000a] = opcode(
                0x000a,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"dec"});

        _opcodes[0x004a] = opcode(
                0x004a,
                2,
                opcode::address_modes::inherent,
                {"deca"});

        _opcodes[0x005a] = opcode(
                0x005a,
                2,
                opcode::address_modes::inherent,
                {"decb"});

        _opcodes[0x0088] = opcode(
                0x0088,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"eora"});

        _opcodes[0x00c8] = opcode(
                0x00c8,
                5,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::immediate
                | opcode::address_modes::relative,
                {"eorb"});

        _opcodes[0x001e] = opcode(
                0x001e,
                8,
                opcode::address_modes::inherent,
                {"exg"});

        _opcodes[0x000c] = opcode(
                0x000c,
                7,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"inc"});

        _opcodes[0x004c] = opcode(
                0x004c,
                2,
                opcode::address_modes::inherent,
                {"inca"});

        _opcodes[0x005c] = opcode(
                0x005c,
                2,
                opcode::address_modes::inherent,
                {"incb"});

        _opcodes[0x000e] = opcode(
                0x000e,
                4,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"jmp"});

        _opcodes[0x008d] = opcode(
                0x008d,
                8,
                opcode::address_modes::extended
                | opcode::address_modes::indexed
                | opcode::address_modes::direct
                | opcode::address_modes::relative,
                {"jsr"});

        _opcodes[0x1024] = opcode(
                0x1024,
                5,
                opcode::address_modes::relative,
                {"lbcc"});

        _opcodes[0x1025] = opcode(
                0x1025,
                5,
                opcode::address_modes::relative,
                {"lbcs"});

        _opcodes[0x1027] = opcode(
                0x1027,
                5,
                opcode::address_modes::relative,
                {"lbeq"});

        _opcodes[0x102c] = opcode(
                0x102c,
                5,
                opcode::address_modes::relative,
                {"lbge"});

        _opcodes[0x102e] = opcode(
                0x102e,
                5,
                opcode::address_modes::relative,
                {"lbgt"});

        _opcodes[0x1022] = opcode(
                0x1022,
                5,
                opcode::address_modes::relative,
                {"lbhi"});

        _opcodes[0x1024] = opcode(
                0x1024,
                5,
                opcode::address_modes::relative,
                {"lbhs"});

        _opcodes[0x102f] = opcode(
                0x102f,
                5,
                opcode::address_modes::relative,
                {"lble"});

        _opcodes[0x1025] = opcode(
                0x1025,
                5,
                opcode::address_modes::relative,
                {"lblo"});

        _opcodes[0x1023] = opcode(
                0x1023,
                5,
                opcode::address_modes::relative,
                {"lbls"});

        _opcodes[0x102d] = opcode(
                0x102d,
                5,
                opcode::address_modes::relative,
                {"lblt"});

        _opcodes[0x102b] = opcode(
                0x102b,
                5,
                opcode::address_modes::relative,
                {"lbmi"});

        _opcodes[0x1026] = opcode(
                0x1026,
                5,
                opcode::address_modes::relative,
                {"lbne"});

        _opcodes[0x102a] = opcode(
                0x102a,
                5,
                opcode::address_modes::relative,
                {"lbpl"});

        _opcodes[0x0016] = opcode(
                0x0016,
                5,
                opcode::address_modes::relative,
                {"lbra"});

        _opcodes[0x1021] = opcode(
                0x1021,
                5,
                opcode::address_modes::relative,
                {"lbrn"});

        _opcodes[0x0017] = opcode(
                0x0017,
                9,
                opcode::address_modes::relative,
                {"lbsr"});

        _opcodes[0x1028] = opcode(
                0x1028,
                9,
                opcode::address_modes::relative,
                {"lbvc"});

        _opcodes[0x1029] = opcode(
                0x1029,
                9,
                opcode::address_modes::relative,
                {"lbvs"});

        _opcodes[0x0086] = opcode(
                0x0086,
                9,
                opcode::address_modes::relative,
                {"lda"});

        _opcodes[0x00c6] = opcode(
                0x00c6,
                9,
                opcode::address_modes::relative,
                {"ldb"});

        _opcodes[0x00cc] = opcode(
                0x00cc,
                9,
                opcode::address_modes::relative,
                {"ldd"});

        _opcodes[0x10ce] = opcode(
                0x10ce,
                9,
                opcode::address_modes::relative,
                {"lds"});

        _opcodes[0x00ce] = opcode(
                0x00ce,
                9,
                opcode::address_modes::relative,
                {"ldu"});

        _opcodes[0x008e] = opcode(
                0x008e,
                9,
                opcode::address_modes::relative,
                {"ldx"});

        _opcodes[0x108e] = opcode(
                0x108e,
                9,
                opcode::address_modes::relative,
                {"ldy"});

        _opcodes[0x0032] = opcode(
                0x0032,
                9,
                opcode::address_modes::relative,
                {"leas"});

        _opcodes[0x0033] = opcode(
                0x0033,
                9,
                opcode::address_modes::relative,
                {"leau"});

        _opcodes[0x0030] = opcode(
                0x0030,
                9,
                opcode::address_modes::relative,
                {"leax"});

        _opcodes[0x0031] = opcode(
                0x0031,
                9,
                opcode::address_modes::relative,
                {"leay"});

        _opcodes[0x0008] = opcode(
                0x0008,
                9,
                opcode::address_modes::relative,
                {"lsl"});

        _opcodes[0x0048] = opcode(
                0x0048,
                9,
                opcode::address_modes::relative,
                {"lsla"});

        _opcodes[0x0058] = opcode(
                0x0058,
                9,
                opcode::address_modes::relative,
                {"lslb"});

        _opcodes[0x0004] = opcode(
                0x0004,
                9,
                opcode::address_modes::relative,
                {"lsr"});

        _opcodes[0x0044] = opcode(
                0x0044,
                9,
                opcode::address_modes::relative,
                {"lsra"});

        _opcodes[0x0054] = opcode(
                0x0054,
                9,
                opcode::address_modes::relative,
                {"lsrb"});

        _opcodes[0x003d] = opcode(
                0x003d,
                9,
                opcode::address_modes::relative,
                {"mul"});

        _opcodes[0x0000] = opcode(
                0x0000,
                9,
                opcode::address_modes::relative,
                {"neg"});

        _opcodes[0x0040] = opcode(
                0x0040,
                9,
                opcode::address_modes::relative,
                {"nega"});

        _opcodes[0x0050] = opcode(
                0x0050,
                9,
                opcode::address_modes::relative,
                {"negb"});

        _opcodes[0x0012] = opcode(
                0x0012,
                9,
                opcode::address_modes::relative,
                {"nop"});

        _opcodes[0x008a] = opcode(
                0x008a,
                9,
                opcode::address_modes::relative,
                {"ora"});

        _opcodes[0x00ca] = opcode(
                0x00ca,
                9,
                opcode::address_modes::relative,
                {"orb"});

        _opcodes[0x001a] = opcode(
                0x001a,
                9,
                opcode::address_modes::relative,
                {"orcc"});

        _opcodes[0x0034] = opcode(
                0x0034,
                9,
                opcode::address_modes::relative,
                {"pshs"});

        _opcodes[0x0036] = opcode(
                0x0036,
                9,
                opcode::address_modes::relative,
                {"pshu"});

        _opcodes[0x0035] = opcode(
                0x0035,
                9,
                opcode::address_modes::relative,
                {"puls"});

        _opcodes[0x0037] = opcode(
                0x0037,
                9,
                opcode::address_modes::relative,
                {"pulu"});

        _opcodes[0x0009] = opcode(
                0x0009,
                9,
                opcode::address_modes::relative,
                {"rol"});

        _opcodes[0x0049] = opcode(
                0x0049,
                9,
                opcode::address_modes::relative,
                {"rola"});

        _opcodes[0x0059] = opcode(
                0x0059,
                9,
                opcode::address_modes::relative,
                {"rolb"});

        _opcodes[0x0006] = opcode(
                0x0006,
                9,
                opcode::address_modes::relative,
                {"ror"});

        _opcodes[0x0046] = opcode(
                0x0046,
                9,
                opcode::address_modes::relative,
                {"rora"});

        _opcodes[0x0056] = opcode(
                0x0056,
                9,
                opcode::address_modes::relative,
                {"rorb"});

        _opcodes[0x003b] = opcode(
                0x003b,
                9,
                opcode::address_modes::relative,
                {"rti"});

        _opcodes[0x0039] = opcode(
                0x0039,
                9,
                opcode::address_modes::relative,
                {"rts"});

        _opcodes[0x0082] = opcode(
                0x0082,
                9,
                opcode::address_modes::relative,
                {"sbca"});

        _opcodes[0x00c2] = opcode(
                0x00c2,
                9,
                opcode::address_modes::relative,
                {"sbcb"});

        _opcodes[0x001d] = opcode(
                0x001d,
                9,
                opcode::address_modes::relative,
                {"sex"});

        _opcodes[0x0087] = opcode(
                0x0087,
                9,
                opcode::address_modes::relative,
                {"sta"});

        _opcodes[0x00c7] = opcode(
                0x00c7,
                9,
                opcode::address_modes::relative,
                {"stb"});

        _opcodes[0x00cd] = opcode(
                0x00cd,
                9,
                opcode::address_modes::relative,
                {"std"});

        _opcodes[0x10cf] = opcode(
                0x10cf,
                9,
                opcode::address_modes::relative,
                {"sts"});

        _opcodes[0x00cf] = opcode(
                0x00cf,
                9,
                opcode::address_modes::relative,
                {"stu"});

        _opcodes[0x008f] = opcode(
                0x008f,
                9,
                opcode::address_modes::relative,
                {"stx"});

        _opcodes[0x108f] = opcode(
                0x108f,
                9,
                opcode::address_modes::relative,
                {"sty"});

        _opcodes[0x0080] = opcode(
                0x0080,
                9,
                opcode::address_modes::relative,
                {"suba"});

        _opcodes[0x00c0] = opcode(
                0x00c0,
                9,
                opcode::address_modes::relative,
                {"subb"});

        _opcodes[0x0083] = opcode(
                0x0083,
                9,
                opcode::address_modes::relative,
                {"subd"});

        _opcodes[0x003f] = opcode(
                0x003f,
                9,
                opcode::address_modes::relative,
                {"swi"});

        _opcodes[0x103f] = opcode(
                0x103f,
                9,
                opcode::address_modes::relative,
                {"swi2"});

        _opcodes[0x113f] = opcode(
                0x113f,
                9,
                opcode::address_modes::relative,
                {"swi3"});

        _opcodes[0x0013] = opcode(
                0x0013,
                9,
                opcode::address_modes::relative,
                {"sync"});

        _opcodes[0x001f] = opcode(
                0x001f,
                9,
                opcode::address_modes::relative,
                {"tfr"});

        _opcodes[0x000d] = opcode(
                0x000d,
                9,
                opcode::address_modes::relative,
                {"tst"});

        _opcodes[0x004d] = opcode(
                0x004d,
                9,
                opcode::address_modes::relative,
                {"tsta"});

        _opcodes[0x005d] = opcode(
                0x005d,
                9,
                opcode::address_modes::relative,
                {"tstb"});

        for (const auto& it : _opcodes) {
            const auto& mnemonics = it.second.mnemonics();
            _mnemonics.insert(mnemonics.begin(), mnemonics.end());
        }
    }

}