// //
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
#include "IrDoubleLiteral.h"
#include "IrBoolLiteral.h"
#include "IrStringLiteral.h"
#include "IrExpression.h"

namespace Decaf
{

enum class IrReg : int
{
    Temp,
    Ret,
    RetWord,
    Output,
    OutputWord,
    
    DoubleTemp,
    DoubleRet,
    
    // General parameters (non double/float)
    Param1,
    Param2,
    Param3,
    Param4,
    Param5,
    Param6,
    
    // Floating-point parameters
    DoubleParam1,
    DoubleParam2,
    DoubleParam3,
    DoubleParam4,
    DoubleParam5,
    DoubleParam6,
    DoubleParam7,
    DoubleParam8,
    
    Base,
    Index,
    
    NUM
};

const std::string g_registerNames[(int)IrReg::NUM] =
{
    // temp registers
    "%r10",
    "%rax",
    "%al",
    "%rdx",
    "%dl",
    
    "%xmm7",
    "%xmm0",
    
    // parameter registers
    "%rdi",
    "%rsi",
    "%rdx",
    "%rcx",
    "%r8",
    "%r9",
    
    // double parameter registers
    "%xmm0",
    "%xmm1",
    "%xmm2",
    "%xmm3",
    "%xmm4",
    "%xmm5",
    "%xmm6",
    "%xmm7",
    
    // addressing register
    "%rbx",
    "%rsi"
};

const std::string g_registerNames_ia32[(int)IrReg::NUM] =
{
    // temp registers
    "%eax",
    "%al",
    "%edx",
    "%dl",
    "%ebx",
    
    "%xmm7",
    "%xmm0",
    
    // parameter registers
    "---",
    "---",
    "---",
    "---",
    "---",
    "---"    

    // double parameter registers
    "---",
    "---",
    "---",
    "---",
    "---",
    "---"    
    "---",
    "---"    
    
    // addressing register
    "%ebx",
    "%esi"    
};

class IrRegister : public IrBase
{
public:
    IrRegister(IrReg which, bool isDouble = false) :
        IrBase(0, 0, __FILE__),
        m_which(which),
        m_isDouble(isDouble) {}
    virtual ~IrRegister() {}
    
    virtual void print(unsigned int depth) {} 
    
    IrReg name() const { return m_which; }
    bool isDouble() const { return m_isDouble; }
    
protected:
    IrReg m_which;
    bool m_isDouble;
};

const std::shared_ptr<IrRegister> g_indexRegister = std::make_shared<IrRegister>(IrReg::Index);

    
const std::string gIrOpcodeStrings[(int)IrOpcode::NUM_OPCODES] =
{
    "NOOP",
    "MOV",
    "LOAD",
    "STORE",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
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
    "IFNZ",
    "PARAM",
    "GETPARAM",
    "STRING",
    "GLOBAL"
};
static_assert(sizeof(gIrOpcodeStrings)/sizeof(std::string) == (size_t)IrOpcode::NUM_OPCODES, "Unexpected number of IrOpcode strings.");

const std::string& IrOpcodeToString(IrOpcode opcode)
{
    return gIrOpcodeStrings[(int)opcode];
}

void IrPrintTacArg(const std::shared_ptr<IrBase> arg, std::ostream& stream)
{
    if (arg == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(arg.get());
    const IrIntegerLiteral* iliteral = dynamic_cast<const IrIntegerLiteral*>(arg.get());
    const IrDoubleLiteral* dliteral = dynamic_cast<const IrDoubleLiteral*>(arg.get());
    const IrBooleanLiteral* bliteral = dynamic_cast<const IrBooleanLiteral*>(arg.get());
    const IrStringLiteral* sliteral = dynamic_cast<const IrStringLiteral*>(arg.get());
    
    if (ident != nullptr)
    {
        stream << "$" << ident->getIdentifier();
    }
    else if (iliteral != nullptr)
    {
        stream << "$" << iliteral->getValue();
    }
    else if (dliteral != nullptr)
    {
        stream << "$" << dliteral->getValue();
    }
    else if (bliteral != nullptr)
    {
        stream << (bliteral->getValue() ? "$1" : "$0");
    }
    else if (sliteral != nullptr)
    {
        stream << sliteral->getValue();
    }
}

void IrPrintTacLabel(const std::shared_ptr<IrBase> label, std::ostream& stream)
{
    if (label == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(label.get());
    
    if (ident != nullptr)
    {
        stream << ident->getIdentifier();
    }
}

void IrPrintTac(const IrTacStmt& stmt, std::ostream& stream)
{
    stream << IrOpcodeToString(stmt.m_opcode) << " ";
    
    if (stmt.m_opcode != IrOpcode::LABEL)
    {
        if (stmt.m_arg0)
        {
            IrPrintTacArg(stmt.m_arg0, stream);
        }
        if (stmt.m_arg1)
        {
            if (stmt.m_arg0) stream << ", ";
            
            IrPrintTacArg(stmt.m_arg1, stream);
        }
        if (stmt.m_arg2)
        {
            stream << ", ";
            IrPrintTacArg(stmt.m_arg2, stream);
        }
    }
    else
    {
        IrPrintTacLabel(stmt.m_arg0, stream); stream << " ";
    }
    
    stream << "\t// " << stmt.m_info << std::endl;
}

static bool g_ia64 = true;

void IrOutputArg(const std::shared_ptr<IrBase> arg, std::ostream& stream)
{
    if (arg == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(arg.get());
    const IrIntegerLiteral* iliteral = dynamic_cast<const IrIntegerLiteral*>(arg.get());    
    const IrDoubleLiteral* dliteral = dynamic_cast<const IrDoubleLiteral*>(arg.get());
    const IrBooleanLiteral* bliteral = dynamic_cast<const IrBooleanLiteral*>(arg.get());
    const IrRegister* reg = dynamic_cast<const IrRegister*>(arg.get());

    if (ident != nullptr)
    {
        if (ident->isLabel())
            stream << "$" << ident->getIdentifier();
        else if (ident->isGlobal())
            stream << ident->getIdentifier();
        else
            stream << "-" << ident->getAddress()+8 << (g_ia64 ? "(%rbp)" : "(%ebp)");
    }
    else if (iliteral != nullptr)
    {
        stream << "$" << iliteral->getValue();
    }
    else if (dliteral != nullptr)
    {
        stream << "$" << dliteral->getValue();
    }
    else if (bliteral != nullptr)
    {
        stream << (bliteral->getValue() ? "$1" : "$0");
    }
    else if (reg != nullptr)
    {
        if (g_ia64)
            stream << g_registerNames[(int)reg->name()];
        else
            stream << g_registerNames_ia32[(int)reg->name()];
    }
}

void IrOutputLabel(const std::shared_ptr<IrBase> arg, std::ostream& stream)
{
    if (arg == nullptr) return;
    
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(arg.get());
    const IrStringLiteral* sliteral = dynamic_cast<const IrStringLiteral*>(arg.get());
    
    if (ident != nullptr)
    {
        stream << ident->getIdentifier();
    }
    else if (sliteral != nullptr)
    {
        stream << sliteral->getValue();
    }
}

const std::shared_ptr<IrRegister> g_tempReg = std::make_shared<IrRegister>(IrReg::Temp);
const std::shared_ptr<IrRegister> g_retReg = std::make_shared<IrRegister>(IrReg::Ret);
const std::shared_ptr<IrRegister> g_retRegWord = std::make_shared<IrRegister>(IrReg::RetWord);
const std::shared_ptr<IrRegister> g_outReg = std::make_shared<IrRegister>(IrReg::Output);
const std::shared_ptr<IrRegister> g_outRegWord = std::make_shared<IrRegister>(IrReg::OutputWord);

const std::shared_ptr<IrRegister> g_tempDoubleReg = std::make_shared<IrRegister>(IrReg::DoubleTemp, true);
const std::shared_ptr<IrRegister> g_retDoubleReg = std::make_shared<IrRegister>(IrReg::DoubleRet, true);

const int NUM_ARG_REGS = 6;
const std::shared_ptr<IrRegister> g_paramRegisters[NUM_ARG_REGS] =
{
    std::make_shared<IrRegister>(IrReg::Param1),
    std::make_shared<IrRegister>(IrReg::Param2),
    std::make_shared<IrRegister>(IrReg::Param3),
    std::make_shared<IrRegister>(IrReg::Param4),
    std::make_shared<IrRegister>(IrReg::Param5),
    std::make_shared<IrRegister>(IrReg::Param6)
};
static int g_nextParamRegister = 0;

const int NUM_DOUBLE_ARG_REGS = 8;
const std::shared_ptr<IrRegister> g_paramDoubleRegisters[NUM_DOUBLE_ARG_REGS] =
{
    std::make_shared<IrRegister>(IrReg::DoubleParam1, true),
    std::make_shared<IrRegister>(IrReg::DoubleParam2, true),
    std::make_shared<IrRegister>(IrReg::DoubleParam3, true),
    std::make_shared<IrRegister>(IrReg::DoubleParam4, true),
    std::make_shared<IrRegister>(IrReg::DoubleParam5, true),
    std::make_shared<IrRegister>(IrReg::DoubleParam6, true),
    std::make_shared<IrRegister>(IrReg::DoubleParam7, true),
    std::make_shared<IrRegister>(IrReg::DoubleParam8, true)
};
static int g_nextDoubleParamRegister = 0;

bool IrIsRegister(const std::shared_ptr<IrBase> value)
{
    const IrRegister* reg = dynamic_cast<const IrRegister*>(value.get());
    return (reg != nullptr);
}

bool IrIsMemory(const std::shared_ptr<IrBase> value)
{
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(value.get());
    return (ident != nullptr);
}

bool IrIsDouble(const std::shared_ptr<IrBase> value)
{
    const IrIdentifier* ident = dynamic_cast<const IrIdentifier*>(value.get());   
    const IrRegister* reg = dynamic_cast<const IrRegister*>(value.get());
    const IrDoubleLiteral* literal = dynamic_cast<const IrDoubleLiteral*>(value.get());
    
    bool isDouble = false;
    
    if (ident != nullptr)
    {
        isDouble = (ident->getType() == IrType::Double);
    }
    else if (reg != nullptr)
    {
        isDouble = reg->isDouble();
    }
    else
    {
        isDouble = (literal != nullptr);
    }
    return isDouble;
}

void IrGenMov(const std::shared_ptr<IrBase> src, const std::shared_ptr<IrBase> dst, std::ostream& stream)
{
    if (IrIsMemory(src) && IrIsMemory(dst))
    {
        stream << (g_ia64 ? "movq " : "movl ");
        IrOutputArg(src, stream);
        stream << ", ";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        stream << (g_ia64 ? "movq " : "movl ");
        IrOutputArg(g_tempReg, stream);
        stream << ", ";
        IrOutputArg(dst, stream);
        stream << std::endl;      
    }
    else
    {
        stream << (g_ia64 ? "movq " : "movl ");
        IrOutputArg(src, stream);
        stream << ", ";
        IrOutputArg(dst, stream);
        stream << std::endl;
    }
}

void IrGenLoad(const std::shared_ptr<IrBase> baseAddr, const std::shared_ptr<IrBase> offset, const std::shared_ptr<IrBase> dst, std::ostream& stream)
{
    const IrIdentifier* baseIdent = dynamic_cast<const IrIdentifier*>(baseAddr.get());   
    const IrIntegerLiteral* offsetLiteral = dynamic_cast<const IrIntegerLiteral*>(offset.get());
    const IrIdentifier* offsetIdent = dynamic_cast<const IrIdentifier*>(offset.get());

    if (offsetIdent != nullptr)
    {
        // load offset value into rsi/esi register
        IrGenMov(offset, g_indexRegister, stream);
    }
    
    stream << (g_ia64 ? "movq " : "movl ");
    
    if (baseIdent->isLabel())
    {
        stream << "$" << baseIdent->getIdentifier();
    }
    else if (baseIdent->isGlobal())
    {
        stream << baseIdent->getIdentifier();
    }
    
    if (offsetLiteral != nullptr)
    {
        stream << "+(" << offsetLiteral->getValue() << "*8), "; 
    }
    else if (offsetIdent != nullptr)
    {
        stream << (g_ia64 ? "(,%rsi,8), " : "(,%esi,4), ");
    }
    IrOutputArg(g_tempReg, stream); 
    stream << std::endl;
    
    stream << (g_ia64 ? "movq " : "movl ");
    IrOutputArg(g_tempReg, stream);
    stream << ", ";
    IrOutputArg(dst, stream);
    stream << std::endl;      
}

void IrGenStore(const std::shared_ptr<IrBase> src, const std::shared_ptr<IrBase> baseAddr, const std::shared_ptr<IrBase> offset, std::ostream& stream)
{
    // Load source value into a register.
    stream << (g_ia64 ? "movq " : "movl ");
    IrOutputArg(src, stream);
    stream << ", ";
    IrOutputArg(g_tempReg, stream);
    stream << std::endl;
    
    const IrIdentifier* baseIdent = dynamic_cast<const IrIdentifier*>(baseAddr.get());   
    const IrIntegerLiteral* offsetLiteral = dynamic_cast<const IrIntegerLiteral*>(offset.get());
    const IrIdentifier* offsetIdent = dynamic_cast<const IrIdentifier*>(offset.get());

    if (offsetIdent != nullptr)
    {
        // load offset value into rsi/esi register
        IrGenMov(offset, g_indexRegister, stream);
    }

    stream << (g_ia64 ? "movq " : "movl ");
    
    IrOutputArg(g_tempReg, stream); 
    stream << ",";
    
    if (baseIdent->isLabel())
    {
        stream << "$" << baseIdent->getIdentifier();
    }
    else if (baseIdent->isGlobal())
    {
        stream << baseIdent->getIdentifier();
    }
    
    if (offsetLiteral != nullptr)
    {
        stream << "+(" << offsetLiteral->getValue() << "*8)"; 
    }
    else if (offsetIdent != nullptr)
    {
        stream << (g_ia64 ? "(,%rsi,8)" : "(,%esi,4)");
    }
    stream << std::endl;
}

void IrGenComparison(const IrTacStmt& stmt, std::ostream& stream)
{    
    // zero the temp output reg
    stream << "xor ";
    IrOutputArg(g_retReg, stream);
    stream << ", ";
    IrOutputArg(g_retReg, stream);
    stream << std::endl;
    
    // make sure the second arg is not an immediate nor a memory access
    IrGenMov(stmt.m_arg0, g_tempReg, stream);
    
    stream << "cmp ";
    IrOutputArg(stmt.m_arg1, stream);
    stream << ", ";
    IrOutputArg(g_tempReg, stream);
    stream << std::endl;  
    
    switch (stmt.m_opcode)
    {
    case IrOpcode::EQUAL:      // arg0 == arg1 -> arg2 (0 or 1)
        stream << "sete ";
        break;
                
    case IrOpcode::NOTEQUAL:   // arg0 != arg1 -> arg2 (0 or 1)
        stream << "setne ";
        break;
        
    case IrOpcode::LESS:       // arg0 < arg1 -> arg2 (0 or 1)
        stream << "setl ";
        break;
        
    case IrOpcode::LESSEQUAL:  // arg0 <= arg1 -> arg2 (0 or 1)
        stream << "setle ";
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
    IrOutputArg(g_retRegWord, stream);    
    stream << std::endl;  
    
    stream << "movzb ";
    IrOutputArg(g_retRegWord, stream);
    stream << ", ";
    IrOutputArg(g_retReg, stream);
    stream << std::endl;
    
    IrGenMov(g_retReg, stmt.m_arg2, stream);      
}

static std::vector<std::shared_ptr<IrBase>> g_extraParams;

void IrGenIA32()
{
    g_ia64 = false;
}

void IrTacGenCode(const IrTacStmt& stmt, std::ostream& stream)
{    
    switch(stmt.m_opcode)
    {
    case IrOpcode::MOV:        // arg0 -> arg2
        IrGenMov(stmt.m_arg0, stmt.m_arg2, stream);
        break;
        
    case IrOpcode::LOAD:       // *[arg0 + arg1] -> arg2
        IrGenLoad(stmt.m_arg0, stmt.m_arg1, stmt.m_arg2, stream);
        break;
        
    case IrOpcode::STORE:      // arg0 -> *[arg1 + arg2]
        IrGenStore(stmt.m_arg0, stmt.m_arg1, stmt.m_arg2, stream);
        break;
        
    case IrOpcode::ADD:        // arg0 + arg1 -> arg2
    case IrOpcode::SUB:        // arg0 - arg1 -> arg2
        IrGenMov(stmt.m_arg0, g_tempReg, stream);
        
        stream << (stmt.m_opcode == IrOpcode::ADD ? "add " : "sub ");
        IrOutputArg(stmt.m_arg1, stream);
        stream << ", ";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        IrGenMov(g_tempReg, stmt.m_arg2, stream);
        break;
        
    case IrOpcode::MUL:        // arg0 * arg1 -> arg2
    case IrOpcode::DIV:        // arg0 / arg1 -> arg2
    case IrOpcode::MOD:        // arg0 % arg1 -> arg2     
        // zero the temp output reg
        stream << "xor ";
        IrOutputArg(g_outReg, stream);
        stream << ", ";
        IrOutputArg(g_outReg, stream);
        stream << std::endl;
        
        IrGenMov(stmt.m_arg0, g_retReg, stream); // arg0 => %rax
        
        // make sure the second arg is not an immediate nor a memory access
        IrGenMov(stmt.m_arg1, g_tempReg, stream);
        
        stream << (stmt.m_opcode == IrOpcode::MUL ? "imul " : "idiv ");
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        if (stmt.m_opcode == IrOpcode::MOD)
            IrGenMov(g_outReg, stmt.m_arg2, stream); // %rdx => arg2            
        else
            IrGenMov(g_retReg, stmt.m_arg2, stream); // %rax => arg2
        break;
         
    case IrOpcode::CALL:       // call arg0 arg1
    
        // Pop extra parameters off the param stack
        if (!g_extraParams.empty())
        {
            // align the stack 16-byte boundary
            if (g_extraParams.size() % 2 == 1)
                stream << "push $0" << std::endl;
            
            for (auto it = g_extraParams.rbegin(); it != g_extraParams.rend(); ++it)
            {
                stream << "push ";
                IrOutputArg(*it, stream);
                stream << std::endl;
            }
            
            g_extraParams.clear();
        }
        stream << "call ";
        IrOutputLabel(stmt.m_arg0, stream);
        stream << std::endl;
        
        if (stmt.m_arg1 != nullptr)
        {
            IrGenMov(g_retReg, stmt.m_arg1, stream);
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
            IrGenMov(stmt.m_arg0, g_retReg, stream);
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
        IrGenMov(stmt.m_arg1, g_tempReg, stream);
    
        stream << (stmt.m_opcode == IrOpcode::AND ? "and " : "or ");
        IrOutputArg(stmt.m_arg0, stream);
        stream << ", ";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
    
        IrGenMov(g_tempReg, stmt.m_arg2, stream);        
        break;
        
    case IrOpcode::NOT:        // !arg1 -> arg2 (0 or 1)
        // make sure the arg is not an immediate 
        IrGenMov(stmt.m_arg1, g_tempReg, stream);
        
        // 'not' is a bit-wise invert 
        stream << "not ";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        // mask all but first bit in result
        stream << "and $1,";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        IrGenMov(g_tempReg, stmt.m_arg2, stream);        
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
        
        stream << "je ";        // jump if cmp == 0
        IrOutputLabel(stmt.m_arg1, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::IFNZ:        // jump if cmp != 0
        stream << "cmp $0, ";       
        IrOutputArg(stmt.m_arg0, stream);
        stream << std::endl;
        
        stream << "jne ";        // jump if cmp != 0
        IrOutputLabel(stmt.m_arg1, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::PARAM:       // push arg0 -> stack
        if (g_ia64 && stmt.m_info < NUM_ARG_REGS)
        {
            IrGenMov(stmt.m_arg0, g_paramRegisters[stmt.m_info], stream);
        }
        else
        {
            g_extraParams.push_back(stmt.m_arg0);
        }
        break;
    
    case IrOpcode::GETPARAM:	// stack -> arg0
        if (g_ia64 && stmt.m_info < NUM_ARG_REGS)
        {
            IrGenMov(g_paramRegisters[stmt.m_info], stmt.m_arg0, stream);
        }
        else
        {
            int argOffset = stmt.m_info - NUM_ARG_REGS;
            
            stream << (g_ia64 ? "movq " : "movl ");
            stream << (argOffset*8+16) << "(%rbp), ";
            IrOutputArg(g_tempReg, stream);
            stream << std::endl;

            stream << (g_ia64 ? "movq " : "movl ");
            IrOutputArg(g_tempReg, stream);
            stream << ", ";
            IrOutputArg(stmt.m_arg0, stream);
            stream << std::endl;
        }
        break;
        
    case IrOpcode::STRING:     // string label -> arg0 value -> arg1
        IrOutputLabel(stmt.m_arg0, stream);
        stream << ":" << std::endl;
        stream << ".string ";
        IrOutputLabel(stmt.m_arg1, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::GLOBAL:
        stream << ".comm ";
        IrOutputLabel(stmt.m_arg0, stream);
        stream << "," << stmt.m_info << ",8" << std::endl;
        break;
        
    default:
        break;
    }
    
}

} // namespace Decaf
