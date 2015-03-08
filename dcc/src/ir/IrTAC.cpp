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
#include <vector>
#include "IrTAC.h"
#include "IrBase.h"
#include "IrIdentifier.h"
#include "IrIntLiteral.h"
#include "IrBoolLiteral.h"
#include "IrStringLiteral.h"

namespace Decaf
{

enum class IrRegName : int
{
    rTemp,
    rRet,
    rRetWord,
    rOutput,
    rOutputWord,
    
    rParam1,
    rParam2,
    rParam3,
    rParam4,
    rParam5,
    rParam6,
    
    NUM
};

const std::string g_registerNames[(int)IrRegName::NUM] =
{
    // temp registers
    "%r10",
    "%rax",
    "%al",
    "%rdx",
    "%dl",
    
    // parameter registers
    "%rdi",
    "%rsi",
    "%rdx",
    "%rcx",
    "%r8",
    "%r9"    
};

class IrRegister : public IrBase
{
public:
    IrRegister(IrRegName which) :
        IrBase(0, 0, __FILE__),
        m_which(which) {}
    virtual ~IrRegister() {}
    
    virtual void print(unsigned int depth) {} 
    
    IrRegName name() const { return m_which; }
    
protected:
    IrRegName m_which;
};

    
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
    "RETURN",
    "EQUAL",
    "NOTEQUAL",
    "LESS",
    "LESSEQUAL",
    "GREATER",
    "GREATEREQUAL",
    "AND",
    "OR",
    "NOT",
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
        std::cout << "$" << ident->getIdentifier();
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

void IrPrintTacLabel(const IrBase* label)
{
    if (label == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(label);
    
    if (ident != nullptr)
    {
        std::cout << ident->getIdentifier();
    }
}

void IrPrintTac(const IrTacStmt& stmt)
{
    std::cout << IrOpcodeToString(stmt.m_opcode) << " ";
    
    if (stmt.m_opcode != IrOpcode::LABEL)
    {
        if (stmt.m_arg0)
        {
            IrPrintTacArg(stmt.m_arg0);
        }
        if (stmt.m_arg1)
        {
            std::cout << ", ";
            IrPrintTacArg(stmt.m_arg1);
        }
        if (stmt.m_arg2)
        {
            std::cout << ", ";
            IrPrintTacArg(stmt.m_arg2);
        }
    }
    else
    {
        IrPrintTacLabel(stmt.m_arg0); std::cout << " ";
    }
    
    std::cout << "\t// " << stmt.m_info << std::endl;
}

void IrOutputArg(const IrBase* arg, std::ostream& stream)
{
if (arg == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(arg);
    const IrIntegerLiteral* iliteral = dynamic_cast<const IrIntegerLiteral*>(arg);
    const IrBooleanLiteral* bliteral = dynamic_cast<const IrBooleanLiteral*>(arg);
    const IrRegister* reg = dynamic_cast<const IrRegister*>(arg);

    if (ident != nullptr)
    {
        if (ident->isLabel())
            stream << "$" << ident->getIdentifier();
        else
            stream << "-" << ident->getAddress()+8 << "(%rbp)";
    }
    else if (iliteral != nullptr)
    {
        stream << "$" << iliteral->getValue();
    }
    else if (bliteral != nullptr)
    {
        stream << (bliteral->getValue() ? "$1" : "$0");
    }
    else if (reg != nullptr)
    {
        stream << g_registerNames[(int)reg->name()];
    }
}

void IrOutputLabel(const IrBase* arg, std::ostream& stream)
{
    if (arg == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(arg);
    const IrStringLiteral* sliteral = dynamic_cast<const IrStringLiteral*>(arg);
    
    if (ident != nullptr)
    {
        stream << ident->getIdentifier();
    }
    else if (sliteral != nullptr)
    {
        stream << sliteral->getValue();
    }
}

const IrRegister g_tempReg(IrRegName::rTemp);
const IrRegister g_retReg(IrRegName::rRet);
const IrRegister g_retRegWord(IrRegName::rRetWord);
const IrRegister g_outReg(IrRegName::rOutput);
const IrRegister g_outRegWord(IrRegName::rOutputWord);

const int NUM_ARG_REGS = 6;
const IrRegister* g_paramRegisters[NUM_ARG_REGS] =
{
    new IrRegister(IrRegName::rParam1),
    new IrRegister(IrRegName::rParam2),
    new IrRegister(IrRegName::rParam3),
    new IrRegister(IrRegName::rParam4),
    new IrRegister(IrRegName::rParam5),
    new IrRegister(IrRegName::rParam6)
};

bool IrIsRegister(const IrBase* value)
{
    const IrRegister* reg = dynamic_cast<const IrRegister*>(value);
    return (reg != nullptr);
}
bool IrIsMemory(const IrBase* value)
{
    const IrIdentifier* reg = dynamic_cast<const IrIdentifier*>(value);
    return (reg != nullptr);
}

void IrGenMov(const IrBase* src, const IrBase* dst, std::ostream& stream)
{
    if (IrIsMemory(src) && IrIsMemory(dst))
    {
        stream << "movq ";
        IrOutputArg(src, stream);
        stream << ", ";
        IrOutputArg(&g_tempReg, stream);
        stream << std::endl;
        
        stream << "movq ";
        IrOutputArg(&g_tempReg, stream);
        stream << ", ";
        IrOutputArg(dst, stream);
        stream << std::endl;      
    }
    else
    {
        stream << "movq ";
        IrOutputArg(src, stream);
        stream << ", ";
        IrOutputArg(dst, stream);
        stream << std::endl;
    }
}

void IrGenComparison(const IrTacStmt& stmt, std::ostream& stream)
{    
    // zero the temp output reg
    stream << "xorq ";
    IrOutputArg(&g_retReg, stream);
    stream << ", ";
    IrOutputArg(&g_retReg, stream);
    stream << std::endl;
    
    // make sure the second arg is not an immediate nor a memory access
    IrGenMov(stmt.m_arg1, &g_tempReg, stream);
    
    stream << "cmp ";
    IrOutputArg(stmt.m_arg0, stream);
    stream << ", ";
    IrOutputArg(&g_tempReg, stream);
    stream << std::endl;  
     
    switch (stmt.m_opcode)
    {
    case IrOpcode::EQUAL:      // arg0 == arg1 -> arg2 (0 or 1)
        stream << "sete ";
        break;
        
    case IrOpcode::LESS:       // arg0 < arg1 -> arg2 (0 or 1)
        stream << "setnge ";
        break;
        
    case IrOpcode::NOTEQUAL:   // arg0 != arg1 -> arg2 (0 or 1)
        stream << "setne ";
        break;
        
    case IrOpcode::LESSEQUAL:  // arg0 <= arg1 -> arg2 (0 or 1)
        stream << "setng ";
        break;
        
    case IrOpcode::GREATER:    // arg0 > arg1 -> arg2 (0 or 1)
        stream << "setg ";
        break;
        
    case IrOpcode::GREATEREQUAL: // arg0 >= arg1 -> arg2 (0 or 1)
        stream << "setge ";
        break;
        
    default:
        break;
    }
    IrOutputArg(&g_retRegWord, stream);    
    stream << std::endl;  
    
    stream << "movzbq ";
    IrOutputArg(&g_retRegWord, stream);
    stream << ", ";
    IrOutputArg(&g_retReg, stream);
    stream << std::endl;
    
    IrGenMov(&g_retReg, stmt.m_arg2, stream);      
}

static std::vector<const IrBase*> g_extraParams;

void IrTacGenCode(const IrTacStmt& stmt, std::ostream& stream)
{    
    switch(stmt.m_opcode)
    {
    case IrOpcode::MOV:        // arg0 -> arg2
        IrGenMov(stmt.m_arg0, stmt.m_arg2, stream);
        break;
        
    case IrOpcode::ADD:        // arg0 + arg1 -> arg2
    case IrOpcode::SUB:        // arg0 - arg1 -> arg2
        IrGenMov(stmt.m_arg0, &g_tempReg, stream);
        
        stream << (stmt.m_opcode == IrOpcode::ADD ? "addq " : "subq ");
        IrOutputArg(stmt.m_arg1, stream);
        stream << ", ";
        IrOutputArg(&g_tempReg, stream);
        stream << std::endl;
        
        IrGenMov(&g_tempReg, stmt.m_arg2, stream);
        break;
        
    case IrOpcode::MUL:        // arg0 * arg1 -> arg2
    case IrOpcode::DIV:        // arg0 / arg1 -> arg2
    case IrOpcode::MOD:        // arg0 % arg1 -> arg2     
        // zero the temp output reg
        stream << "xorq ";
        IrOutputArg(&g_outReg, stream);
        stream << ", ";
        IrOutputArg(&g_outReg, stream);
        stream << std::endl;
        
        IrGenMov(stmt.m_arg0, &g_retReg, stream); // arg0 => %rax
        
        // make sure the second arg is not an immediate nor a memory access
        IrGenMov(stmt.m_arg1, &g_tempReg, stream);
        
        stream << (stmt.m_opcode == IrOpcode::MUL ? "imulq " : "idivq ");
        IrOutputArg(&g_tempReg, stream);
        stream << std::endl;
        
        if (stmt.m_opcode == IrOpcode::MOD)
            IrGenMov(&g_outReg, stmt.m_arg2, stream); // %rdx => arg2            
        else
            IrGenMov(&g_retReg, stmt.m_arg2, stream); // %rax => arg2
        break;
        
    case IrOpcode::LOAD:       // *[arg0 + arg1] -> arg2
        break;
        
    case IrOpcode::STORE:      // arg0 -> *[arg1 + arg2]
        break;
        
    case IrOpcode::CALL:       // call arg0 arg1
    
		// Pop extra parameters off the param stack
		if (!g_extraParams.empty())
		{
			for (auto it = g_extraParams.begin(); it != g_extraParams.end(); ++it)
			{
				stream << "push ";
				IrOutputLabel(*it, stream);
				stream << std::endl;
			}
			g_extraParams.clear();
		}
        stream << "call ";
        IrOutputLabel(stmt.m_arg0, stream);
        stream << std::endl;
        
        if (stmt.m_arg1 != nullptr)
        {
			IrGenMov(&g_retReg, stmt.m_arg1, stream);
		}
        break;
        
    case IrOpcode::FBEGIN:     // begin function
        stream << ".global ";
        IrOutputLabel(stmt.m_arg0, stream);
        stream << std::endl;
        IrOutputLabel(stmt.m_arg0, stream);
        stream << ":" << std::endl; 
        stream << "enter $" << stmt.m_info << ", $0" << std::endl;
        break;
        
    case IrOpcode::RETURN:     // return |arg0|
		if (stmt.m_arg0 != nullptr)
		{
			IrGenMov(stmt.m_arg0, &g_retReg, stream);
		}
        stream << "leave" << std::endl;
        stream << "ret" << std::endl;
        break;
        
    case IrOpcode::EQUAL:      // arg0 == arg1 -> arg2 (0 or 1)
    case IrOpcode::LESS:       // arg0 < arg1 -> arg2 (0 or 1)
    case IrOpcode::NOTEQUAL:   // arg0 != arg1 -> arg2 (0 or 1)
    case IrOpcode::LESSEQUAL:  // arg0 <= arg1 -> arg2 (0 or 1)
    case IrOpcode::GREATER:    // arg0 > arg1 -> arg2 (0 or 1)
    case IrOpcode::GREATEREQUAL: // arg0 >= arg1 -> arg2 (0 or 1)
        IrGenComparison(stmt, stream);
        break;
        
    case IrOpcode::AND:        // arg0 && arg1 -> arg2 (0 or 1)
    case IrOpcode::OR:         // arg0 || arg1 -> arg2 (0 or 1)
        // make sure the second arg is not an immediate nor a memory access
        IrGenMov(stmt.m_arg1, &g_tempReg, stream);
     
        stream << (stmt.m_opcode == IrOpcode::AND ? "andq " : "orq ");
        IrOutputArg(stmt.m_arg0, stream);
        stream << ", ";
        IrOutputArg(&g_tempReg, stream);
        stream << std::endl;
    
        IrGenMov(&g_tempReg, stmt.m_arg2, stream);        
        break;
        
    case IrOpcode::NOT:        // !arg0 -> arg2 (0 or 1)
        // make sure the arg is not an immediate 
        IrGenMov(stmt.m_arg0, &g_tempReg, stream);
            
        stream << "notq ";
        IrOutputArg(&g_tempReg, stream);
        stream << std::endl;
        
        IrGenMov(&g_tempReg, stmt.m_arg2, stream);        
        break;
        
    case IrOpcode::LABEL:      // arg0:
        IrOutputLabel(stmt.m_arg0, stream);
        stream << ":" << std::endl;
        break;
        
    case IrOpcode::JUMP:       // jump arg0
        stream << "jmp ";
        IrOutputLabel(stmt.m_arg0, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::IFZ:        // branch arg0 == 0 to arg1
        stream << "cmp $0, ";       
        IrOutputArg(stmt.m_arg0, stream);
        stream << std::endl;
        
        stream << "jz ";        // jump if cmp == 0
        IrOutputLabel(stmt.m_arg1, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::PARAM:       // push arg0 -> stack
        if (stmt.m_info < NUM_ARG_REGS)
        {
            IrGenMov(stmt.m_arg0, g_paramRegisters[stmt.m_info], stream);
        }
        else
        {
            g_extraParams.push_back(stmt.m_arg0);
        }
        break;
        
    case IrOpcode::STRING:     // string label -> arg0 value -> arg1
        IrOutputLabel(stmt.m_arg0, stream);
        stream << ":" << std::endl;
        stream << ".string ";
        IrOutputLabel(stmt.m_arg1, stream);
        stream << std::endl;
        break;
        
    default:
        break;
    }
    
}

} // namespace Decaf
