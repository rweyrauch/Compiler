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
#include <iostream>
#include "IrTAC.h"
#include "IrIdentifier.h"
#include "IrIntLiteral.h"
#include "IrBoolLiteral.h"
#include "IrStringLiteral.h"

namespace Decaf
{

const std::string gIrOpcodeStrings[(int)IrOpcode::NUM_OPCODES] =
{
    "NOOP",
    "MOV",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "LOAD",
    "STORE",
    "CALL",
    "FBEGIN",
    "FEND",
    "RETURN",
    "EQUAL",
    "LESS",
    "AND",
    "OR",
    "LABEL",
    "JUMP",
    "IFZ",
    "PUSH",
    "POP",
    "STRING"
};
static_assert(sizeof(gIrOpcodeStrings)/sizeof(std::string) == (size_t)IrOpcode::NUM_OPCODES, "Unexpected number of IrOpcode strings.");

const std::string& IrOpcodeToString(IrOpcode opcode)
{
    return gIrOpcodeStrings[(int)opcode];
}

void IrPrintTacArg(const IrBase* arg)
{
    if (arg == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(arg);
    const IrIntegerLiteral* iliteral = dynamic_cast<const IrIntegerLiteral*>(arg);
    const IrBooleanLiteral* bliteral = dynamic_cast<const IrBooleanLiteral*>(arg);
    const IrStringLiteral* sliteral = dynamic_cast<const IrStringLiteral*>(arg);
    
    if (ident != nullptr)
    {
        std::cout << ident->getIdentifier() << " ";
    }
    else if (iliteral != nullptr)
    {
        std::cout << "$" << iliteral->getValue() << " ";
    }
    else if (bliteral != nullptr)
    {
        std::cout << (bliteral->getValue() ? "$1" : "$0") << " ";
    }
    else if (sliteral != nullptr)
    {
        std::cout << sliteral->getValue() << " ";
    }
}

void IrPrintTac(const IrTacStmt& stmt)
{
    std::cout << IrOpcodeToString(stmt.m_opcode) << " ";
    
    IrPrintTacArg(stmt.m_arg0);
    IrPrintTacArg(stmt.m_arg1);
    IrPrintTacArg(stmt.m_arg2);
    
    std::cout << std::endl;
}

} // namespace Decaf
