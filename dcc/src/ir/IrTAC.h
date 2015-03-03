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
    CALL,       // call arg0
    FBEGIN,     // begin function
    FEND,       // end function
    RETURN,     // return |arg0|
    EQUAL,      // arg0 == arg1 -> arg2 (0 or 1)
    LESS,       // arg0 < arg1 -> arg2 (0 or 1)
    AND,        // arg0 && arg1 -> arg2 (0 or 1)
    OR,         // arg0 || arg1 -> arg2 (0 or 1)
    LABEL,      // arg0:
    JUMP,       // jump arg0
    IFZ,        // branch arg0 == 0 to arg1
    PUSH,       // push arg0 -> stack
    POP,        // pop stack -> arg0
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
        m_arg2(nullptr) 
    {}
        
    IrOpcode m_opcode;
    IrBase* m_arg0, *m_arg1, *m_arg2;
};

void IrPrintTac(const IrTacStmt& stmt);

void IrTacGenCode(const IrTacStmt& stmt);

} // namespace Decaf
