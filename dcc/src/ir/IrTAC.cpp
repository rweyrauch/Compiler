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
    "PARAM",
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
        std::cout << ident->getIdentifier();
    }
    else if (iliteral != nullptr)
    {
        std::cout << "$" << iliteral->getValue();
    }
    else if (bliteral != nullptr)
    {
        std::cout << (bliteral->getValue() ? "$1" : "$0");
    }
    else if (sliteral != nullptr)
    {
        std::cout << sliteral->getValue();
    }
}

void IrPrintTac(const IrTacStmt& stmt)
{
    std::cout << IrOpcodeToString(stmt.m_opcode) << " ";
    
    IrPrintTacArg(stmt.m_arg0); std::cout << " ";
    IrPrintTacArg(stmt.m_arg1); std::cout << " ";
    IrPrintTacArg(stmt.m_arg2); std::cout << " ";
    
    std::cout << stmt.m_info << std::endl;
}

void IrOutputArg(const IrBase* arg)
{
    IrPrintTacArg(arg);
}

const int NUM_ARG_REGS = 6;
const std::string gArgumentRegisters[NUM_ARG_REGS] =
{
	"%rdi",
	"%rsi",
	"%rdx",
	"%rcx",
	"%r8",
	"%r9"
};

void IrTacGenCode(const IrTacStmt& stmt)
{    
    switch(stmt.m_opcode)
    {
    case IrOpcode::MOV:        // arg0 -> arg2
        break;
        
    case IrOpcode::ADD:        // arg0 + arg1 -> arg2
        break;
        
    case IrOpcode::SUB:        // arg0 - arg1 -> arg2
        break;
        
    case IrOpcode::MUL:        // arg0 * arg1 -> arg2
        break;
        
    case IrOpcode::DIV:        // arg0 / arg1 -> arg2
        break;
        
    case IrOpcode::MOD:        // arg0 % arg1 -> arg2
        break;
        
    case IrOpcode::LOAD:       // *[arg0 + arg1] -> arg2
        break;
        
    case IrOpcode::STORE:      // arg0 -> *[arg1 + arg2]
        break;
        
    case IrOpcode::CALL:       // call arg0
        std::cout << "call ";
        IrOutputArg(stmt.m_arg0);
        std::cout << std::endl;
        break;
        
    case IrOpcode::FBEGIN:     // begin function
        std::cout << ".global ";
        IrOutputArg(stmt.m_arg0);
        std::cout << std::endl;
        IrOutputArg(stmt.m_arg0);
        std::cout << ":" << std::endl; 
        std::cout << "enter $" << stmt.m_info << ", $0" << std::endl;
        break;
        
    case IrOpcode::FEND:       // end function
        std::cout << "leave" << std::endl;
        break;
        
    case IrOpcode::RETURN:     // return |arg0|
        std::cout << "ret" << std::endl;
        break;
        
    case IrOpcode::EQUAL:      // arg0 == arg1 -> arg2 (0 or 1)
        break;
        
    case IrOpcode::LESS:       // arg0 < arg1 -> arg2 (0 or 1)
        break;
        
    case IrOpcode::AND:        // arg0 && arg1 -> arg2 (0 or 1)
        break;
        
    case IrOpcode::OR:         // arg0 || arg1 -> arg2 (0 or 1)
        break;
        
    case IrOpcode::LABEL:      // arg0:
        IrOutputArg(stmt.m_arg0);
        std::cout << ":" << std::endl;
        break;
        
    case IrOpcode::JUMP:       // jump arg0
        break;
        
    case IrOpcode::IFZ:        // branch arg0 == 0 to arg1
        break;
        
    case IrOpcode::PARAM:       // push arg0 -> stack
        std::cout << "movq ";
        std::cout << "$";
        IrOutputArg(stmt.m_arg0);
        if (stmt.m_info < NUM_ARG_REGS)
        {
			std::cout << ", " << gArgumentRegisters[stmt.m_info] << std::endl;
		}
		else
		{
			std::cout << "*** OUT OF REGISTERS ****" << std::endl;
		}
        break;
        
    case IrOpcode::STRING:     // string label -> arg0 value -> arg1
        IrOutputArg(stmt.m_arg0);
        std::cout << ":" << std::endl;
        std::cout << ".string ";
        IrOutputArg(stmt.m_arg1);
        std::cout << std::endl;
        break;
        
    default:
        break;
    }
    
}

} // namespace Decaf
