//
// The MIT License (MIT)
//
// Copyright (c) 2015 Rick Weyrauch (rpweyrauch@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once
#include <string>
#include <iostream>

namespace Decaf
{
class IrBase;

enum class IrOpcode : int
{
    NOOP = 0,
    MOV,        // arg0 -> arg2
    ADD,        // arg0 + arg1 -> arg2
    SUB,        // arg0 - arg1 -> arg2
    MUL,        // arg0 * arg1 -> arg2
    DIV,        // arg0 / arg1 -> arg2
    MOD,        // arg0 % arg1 -> arg2
    LOAD,       // *[arg0 + arg1] -> arg2
    STORE,      // arg0 -> *[arg1 + arg2]
    CALL,       // call arg0 arg1
    FBEGIN,     // begin function
    RETURN,     // return |arg0|
    EQUAL,      // arg0 == arg1 -> arg2 (0 or 1)
    NOTEQUAL,   // arg0 != arg1 -> arg2 (0 or 1)
    LESS,       // arg0 < arg1 -> arg2 (0 or 1)
    LESSEQUAL,  // arg0 <= arg1 -> arg2 (0 or 1)
    GREATER,    // arg0 > arg1 -> arg2 (0 or 1)
    GREATEREQUAL, // arg0 >= arg1 -> arg2 (0 or 1)
    AND,        // arg0 && arg1 -> arg2 (0 or 1)
    OR,         // arg0 || arg1 -> arg2 (0 or 1)
    NOT,        // !arg0 -> arg2 (0 or 1)
    LABEL,      // arg0:
    JUMP,       // jump arg0
    IFZ,        // branch arg0 == 0 to arg1
    PARAM,      // function call param, arg0 => ident info => argument number
    STRING,     // string label -> arg0 value -> arg1
    
    NUM_OPCODES
};

const std::string& IrOpcodeToString(IrOpcode opcode);

struct IrTacStmt
{
    IrTacStmt() :
        m_opcode(IrOpcode::NOOP),
        m_arg0(nullptr),
        m_arg1(nullptr),
        m_arg2(nullptr),
        m_info(0)
    {}
        
    IrOpcode m_opcode;
    IrBase* m_arg0, *m_arg1, *m_arg2;
    int m_info;
};

void IrPrintTac(const IrTacStmt& stmt);

void IrTacGenCode(const IrTacStmt& stmt, std::ostream& stream = std::cout);

} // namespace Decaf
