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

void IrTacArg::build(const IrIdentifier* ident)
{
    if (ident != nullptr)
    {
        if (ident->isLabel())
            m_usage = IrUsage::Label;
        else if (ident->isGlobal())
            m_usage = IrUsage::Global;
        else
            m_usage = IrUsage::Identifier;
            
        switch (ident->getType())
        {
            case IrType::Integer:
                m_type = IrArgType::Integer;
                break;
            case IrType::Double:
                m_type = IrArgType::Double;
                break;
            case IrType::Boolean:
                m_type = IrArgType::Boolean;
                break;
            default:
                m_type = IrArgType::String;
                break;
        }
        m_value.m_address = ident->getAddress();
        m_asString = ident->getIdentifier();
    }
}

void IrTacArg::build(const IrLiteral* literal)
{
    const IrIntegerLiteral* iliteral = dynamic_cast<const IrIntegerLiteral*>(literal);
    const IrDoubleLiteral* dliteral = dynamic_cast<const IrDoubleLiteral*>(literal);
    const IrBooleanLiteral* bliteral = dynamic_cast<const IrBooleanLiteral*>(literal);
    const IrStringLiteral* sliteral = dynamic_cast<const IrStringLiteral*>(literal);
    if (iliteral)
        build(iliteral);
    else if (dliteral)
        build(dliteral);
    else if (bliteral)
        build(bliteral);
    else if (sliteral)
        build(sliteral);
}

void IrTacArg::build(const IrIntegerLiteral* literal)
{
    if (literal != nullptr)
    {
        m_usage = IrUsage::Literal;
        m_isConstant = true;
        m_type = IrArgType::Integer;
        m_value.m_int = literal->getValue();
        m_asString = literal->getValueAsString();
    }
}

void IrTacArg::build(const IrDoubleLiteral* literal)
{
    if (literal != nullptr)
    {
        m_usage = IrUsage::Literal;
        m_isConstant = true;
        m_type = IrArgType::Double;
        m_value.m_double = literal->getValue();
        m_asString = literal->getValueAsString();
    }
}

void IrTacArg::build(const IrBooleanLiteral* literal)
{
    if (literal != nullptr)
    {
        m_usage = IrUsage::Literal;
        m_isConstant = true;
        m_type = IrArgType::Boolean;
        m_value.m_int = literal->getValue() ? 1 : 0;
        m_asString = literal->getValue() ? "1" : "0";
    }
}

void IrTacArg::build(const IrStringLiteral* literal)
{
    if (literal != nullptr)
    {
        m_usage = IrUsage::Literal;
        m_isConstant = true;
        m_type = IrArgType::String;
        m_asString = literal->getValue();
    }
}

bool IrTacStmt::hasSrc0() const
{
    return (m_src0.m_usage != IrUsage::Unused);
}

bool IrTacStmt::hasSrc1() const
{
    return (m_src1.m_usage != IrUsage::Unused);
}

bool IrTacStmt::hasDst() const
{
    return (m_dst.m_usage != IrUsage::Unused);
}

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

IrTacArg makeRegister(IrArgType type, IrReg which)
{
    IrTacArg arg;
    
    arg.m_usage = IrUsage::Register;
    arg.m_type = type;
    arg.m_value.m_int = (int)which;
    arg.m_asString = g_registerNames[(int)which];
    arg.m_isConstant = false;
    arg.m_valueNumber = 0;
    
    return arg;
}

const IrTacArg g_indexRegister = makeRegister(IrArgType::Integer, IrReg::Index);

void IrPrintTacArg(const IrTacArg& arg, std::ostream& stream)
{
    if ((arg.m_usage == IrUsage::Identifier) ||
        (arg.m_usage == IrUsage::Literal) ||
        (arg.m_usage == IrUsage::Global) ||
        (arg.m_usage == IrUsage::Label))
    {
        stream << "$" << arg.m_asString;
    }
    else
    {
	stream << "Unexpected arg usage " << (int)arg.m_usage << "  " << arg.m_asString;
    }
}

void IrPrintTacLabel(const IrTacArg& label, std::ostream& stream)
{
    if (label.m_usage == IrUsage::Label)
    {
        stream << label.m_asString;
    }
}

void IrPrintTac(const IrTacStmt& stmt, std::ostream& stream)
{
     stream << IrOpcodeToString(stmt.m_opcode) << " ";
    
    if (stmt.m_opcode != IrOpcode::LABEL)
    {
        if (stmt.hasSrc0())
            IrPrintTacArg(stmt.m_src0, stream);
        
        if (stmt.hasSrc1())
        {
            if (stmt.hasSrc0()) { stream << ", "; }
            
            IrPrintTacArg(stmt.m_src1, stream);
        }
        if (stmt.hasDst())
        {
            stream << ", ";
            IrPrintTacArg(stmt.m_dst, stream);
        }
    }
    else
    {
        IrPrintTacLabel(stmt.m_src0, stream); stream << " ";
    }
    
    stream << "\t// " << stmt.m_info << std::endl;
}

static bool g_ia64 = true;

void IrOutputArg(const IrTacArg& arg, std::ostream& stream)
{
    switch (arg.m_usage)
    {
    case IrUsage::Label:
        stream << "$" << arg.m_asString;
        break;
    case IrUsage::Global:
        stream << arg.m_asString;
        break;
    case IrUsage::Identifier:
    case IrUsage::Argument:
        stream << "-" << arg.m_value.m_address+8 << (g_ia64 ? "(%rbp)" : "(%ebp)");       
        break;
    case IrUsage::Literal:
        stream << "$" << arg.m_asString;
        break;
    case IrUsage::Register:
        if (g_ia64)
            stream << g_registerNames[arg.m_value.m_int];
        else
            stream << g_registerNames_ia32[arg.m_value.m_int];
        break;
    default:
        // Unexpected.
        break;
    }
}

void IrOutputLabel(const IrTacArg& arg, std::ostream& stream)
{
    stream << arg.m_asString;
}

const IrTacArg g_tempReg = makeRegister(IrArgType::Integer, IrReg::Temp);
const IrTacArg g_retReg = makeRegister(IrArgType::Integer, IrReg::Ret);
const IrTacArg g_retRegWord = makeRegister(IrArgType::Integer, IrReg::RetWord);
const IrTacArg g_outReg = makeRegister(IrArgType::Integer, IrReg::Output);
const IrTacArg g_outRegWord = makeRegister(IrArgType::Integer, IrReg::OutputWord);
const IrTacArg g_tempDoubleReg = makeRegister(IrArgType::Double, IrReg::DoubleTemp);
const IrTacArg g_retDoubleReg = makeRegister(IrArgType::Double, IrReg::DoubleRet);

const int NUM_ARG_REGS = 6;
const IrTacArg g_paramRegisters[NUM_ARG_REGS] =
{
    makeRegister(IrArgType::Integer, IrReg::Param1),
    makeRegister(IrArgType::Integer, IrReg::Param2),
    makeRegister(IrArgType::Integer, IrReg::Param3),
    makeRegister(IrArgType::Integer, IrReg::Param4),
    makeRegister(IrArgType::Integer, IrReg::Param5),
    makeRegister(IrArgType::Integer, IrReg::Param6),
};

const int NUM_DOUBLE_ARG_REGS = 8;
const IrTacArg g_paramDoubleRegisters[NUM_DOUBLE_ARG_REGS] =
{
    makeRegister(IrArgType::Double, IrReg::DoubleParam1),
    makeRegister(IrArgType::Double, IrReg::DoubleParam2),
    makeRegister(IrArgType::Double, IrReg::DoubleParam3),
    makeRegister(IrArgType::Double, IrReg::DoubleParam4),
    makeRegister(IrArgType::Double, IrReg::DoubleParam5),
    makeRegister(IrArgType::Double, IrReg::DoubleParam6),
    makeRegister(IrArgType::Double, IrReg::DoubleParam7),
    makeRegister(IrArgType::Double, IrReg::DoubleParam8)
};

bool IrIsRegister(const IrTacArg& arg)
{
    return (arg.m_usage == IrUsage::Register);
}
bool IrIsMemory(const IrTacArg& arg)
{
    return (arg.m_usage == IrUsage::Identifier) || (arg.m_usage == IrUsage::Global);
}
bool IrIsDouble(const IrTacArg& arg)
{
    return (arg.m_type == IrArgType::Double);
}

void IrGenMov(const IrTacArg& src, const IrTacArg& dst, std::ostream& stream)
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

void IrGenLoad(const IrTacArg& baseAddr, const IrTacArg& offset, const IrTacArg& dst, std::ostream& stream)
{
    // load offset value into rsi/esi register
    IrGenMov(offset, g_indexRegister, stream);

    // TODO: add bounds check to offset argument, range in offset.m_info
        
    stream << (g_ia64 ? "movq " : "movl ");
    
    if (baseAddr.m_usage == IrUsage::Label)
    {
        stream << "$" << baseAddr.m_asString;
    }
    else if (baseAddr.m_usage == IrUsage::Global)
    {
        stream << baseAddr.m_asString;
    }
    
    if (offset.m_usage == IrUsage::Literal)
    {
        stream << "+(" << offset.m_value.m_address << "*8), "; 
    }
    else if (offset.m_usage == IrUsage::Identifier)
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

void IrGenStore(const IrTacArg& src, const IrTacArg& baseAddr, const IrTacArg& offset, std::ostream& stream)
{
    // Load source value into a register.
    stream << (g_ia64 ? "movq " : "movl ");
    IrOutputArg(src, stream);
    stream << ", ";
    IrOutputArg(g_tempReg, stream);
    stream << std::endl;

    // load offset value into rsi/esi register
    IrGenMov(offset, g_indexRegister, stream);

    // TODO: add bounds check to offset argument, range in offset.m_info
        
    stream << (g_ia64 ? "movq " : "movl ");
    
    IrOutputArg(g_tempReg, stream); 
    stream << ",";
    
    if (baseAddr.m_usage == IrUsage::Label)
    {
        stream << "$" << baseAddr.m_asString;
    }
    else if (baseAddr.m_usage == IrUsage::Global)
    {
        stream << baseAddr.m_asString;
    }
    
    if (offset.m_usage == IrUsage::Literal)
    {
        stream << "+(" << offset.m_value.m_address << "*8)"; 
    }
    else if (offset.m_usage == IrUsage::Identifier)
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
    IrGenMov(stmt.m_src0, g_tempReg, stream);
    
    stream << "cmp ";
    IrOutputArg(stmt.m_src1, stream);
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
    
    IrGenMov(g_retReg, stmt.m_dst, stream);      
}

static std::vector<IrTacArg> g_funcCallParams;

void IrGenParamPush(std::ostream& stream)
{
    std::vector<IrTacArg> extraParams;
    
    if (g_ia64)
    {
        int nextDoubleRegister = 0;
        int nextNormalRegister = 0;

        for (auto it : g_funcCallParams)
        {
            if (IrIsDouble(it))
            {
                if (nextDoubleRegister < NUM_DOUBLE_ARG_REGS)
                {
                    IrGenMov(it, g_paramDoubleRegisters[nextDoubleRegister], stream);
                    nextDoubleRegister++;
                }
                else
                {
                    extraParams.push_back(it);
                }
            }
            else
            {
                if (nextNormalRegister < NUM_ARG_REGS)
                {
                    IrGenMov(it, g_paramRegisters[nextNormalRegister], stream);
                    nextNormalRegister++;
                }
                else
                {
                    extraParams.push_back(it);
                }
            }       
        }
    }
    else
    {
        extraParams = g_funcCallParams;
    }
    
    // Pop extra parameters off the param stack
    if (!extraParams.empty())
    {
        // align the stack 16-byte boundary
        if (extraParams.size() % 2 == 1)
            stream << "push $0" << std::endl;
        
        for (auto it = extraParams.rbegin(); it != extraParams.rend(); ++it)
        {
            stream << "push ";
            IrOutputArg(*it, stream);
            stream << std::endl;
        }
    }   
        
    g_funcCallParams.clear();
}

void IrGenIA32()
{
    g_ia64 = false;
}

static int g_nextUsedDoubleParamReg = 0;
static int g_nextUsedNormalParamReg = 0;
static int g_nextParamStackLocation = 0;

void IrTacGenCode(const IrTacStmt& stmt, std::ostream& stream)
{    
    switch(stmt.m_opcode)
    {
    case IrOpcode::MOV:        // arg0 -> arg2
        IrGenMov(stmt.m_src0, stmt.m_dst, stream);
        break;
        
    case IrOpcode::LOAD:       // *[arg0 + arg1] -> arg2
        IrGenLoad(stmt.m_src0, stmt.m_src1, stmt.m_dst, stream);
        break;
        
    case IrOpcode::STORE:      // arg0 -> *[arg1 + arg2]
        IrGenStore(stmt.m_src0, stmt.m_src1, stmt.m_dst, stream);
        break;
        
    case IrOpcode::ADD:        // arg0 + arg1 -> arg2
    case IrOpcode::SUB:        // arg0 - arg1 -> arg2
        IrGenMov(stmt.m_src0, g_tempReg, stream);
        
        stream << (stmt.m_opcode == IrOpcode::ADD ? "add " : "sub ");
        IrOutputArg(stmt.m_src1, stream);
        stream << ", ";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        IrGenMov(g_tempReg, stmt.m_dst, stream);
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
        
        IrGenMov(stmt.m_src0, g_retReg, stream); // arg0 => %rax
        
        // make sure the second arg is not an immediate nor a memory access
        IrGenMov(stmt.m_src1, g_tempReg, stream);
        
        stream << (stmt.m_opcode == IrOpcode::MUL ? "imul " : "idiv ");
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        if (stmt.m_opcode == IrOpcode::MOD)
        {
            IrGenMov(g_outReg, stmt.m_dst, stream); // %rdx => arg2                       
        }
        else
        {
            IrGenMov(g_retReg, stmt.m_dst, stream); // %rax => arg2
        }
        break;
        
    case IrOpcode::CALL:       // call arg0 arg1
         IrGenParamPush(stream);

        stream << "call ";
        IrOutputLabel(stmt.m_src0, stream);
        stream << std::endl;
         
        if (stmt.hasSrc1())
        {
            IrGenMov(g_retReg, stmt.m_src1, stream);
        }
        break;
        
    case IrOpcode::FBEGIN:     // begin function
        stream << ".global ";
        IrOutputLabel(stmt.m_src0, stream);
        stream << std::endl;
        IrOutputLabel(stmt.m_src0, stream);
        stream << ":" << std::endl; 
        stream << "enter $" << stmt.m_info << ", $0" << std::endl;
        
        g_nextUsedDoubleParamReg = 0;
        g_nextUsedNormalParamReg = 0;
        g_nextParamStackLocation = 0;
        break;
        
    case IrOpcode::RETURN:     // return |arg0|
        if (stmt.hasSrc0())
        {
            IrGenMov(stmt.m_src0, g_retReg, stream);
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
        IrGenMov(stmt.m_src1, g_tempReg, stream);
    
        stream << (stmt.m_opcode == IrOpcode::AND ? "and " : "or ");
        IrOutputArg(stmt.m_src0, stream);
        stream << ", ";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        IrGenMov(g_tempReg, stmt.m_dst, stream);
        break;
        
    case IrOpcode::NOT:        // !arg1 -> arg2 (0 or 1)
        // make sure the arg is not an immediate 
         IrGenMov(stmt.m_src1, g_tempReg, stream);
        
        // 'not' is a bit-wise invert 
        stream << "not ";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        // mask all but first bit in result
        stream << "and $1,";
        IrOutputArg(g_tempReg, stream);
        stream << std::endl;
        
        IrGenMov(g_tempReg, stmt.m_dst, stream);
        break;
        
    case IrOpcode::LABEL:      // arg0:
        IrOutputLabel(stmt.m_src0, stream);
        stream << ":" << std::endl;     
        break;
        
    case IrOpcode::JUMP:       // jump arg0
        stream << "jmp ";
        IrOutputLabel(stmt.m_src0, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::IFZ:        // branch arg0 == 0 to arg1
        if (stmt.m_src0.m_usage != IrUsage::Literal)
        {    
            stream << "cmp $0, ";       
            IrOutputArg(stmt.m_src0, stream);
            stream << std::endl;           
         }
        else // literal
        {
            IrGenMov(stmt.m_src0, g_tempReg, stream);
            
            stream << "cmp $0, ";       
            IrOutputArg(g_tempReg, stream);
            stream << std::endl;                       
        }
        stream << "jz ";        // jump if cmp == 0
        IrOutputLabel(stmt.m_src1, stream);
        stream << std::endl;
       break;
        
    case IrOpcode::IFNZ:        // jump if cmp != 0
        if (stmt.m_src0.m_usage != IrUsage::Literal)
        {    
            stream << "cmp $0, ";       
            IrOutputArg(stmt.m_src0, stream);
            stream << std::endl;
        }
        else
        {
            IrGenMov(stmt.m_src0, g_tempReg, stream);
            
            stream << "cmp $0, ";       
            IrOutputArg(g_tempReg, stream);
            stream << std::endl;                       
        }
        
        stream << "jnz ";        // jump if cmp != 0
        IrOutputLabel(stmt.m_src1, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::PARAM:       // push arg0 -> stack
        g_funcCallParams.push_back(stmt.m_src0);
        break;
    
    case IrOpcode::GETPARAM:	// stack -> arg0
        if (g_ia64)
        {
            bool onStack = false;
            if (IrIsDouble(stmt.m_src0))
            {
                if (g_nextUsedDoubleParamReg < NUM_DOUBLE_ARG_REGS)
                {
                    IrGenMov(g_paramDoubleRegisters[g_nextUsedDoubleParamReg], stmt.m_src0, stream);
                    g_nextUsedDoubleParamReg++;
                }
                else
                {
                    onStack = true;
                }
            }
            else
            {
                if (g_nextUsedNormalParamReg < NUM_ARG_REGS)
                {
                    IrGenMov(g_paramRegisters[g_nextUsedNormalParamReg], stmt.m_src0, stream);
                    g_nextUsedNormalParamReg++;
                }
                else
                {
                    onStack = true;
                }
            }
            if (onStack)
            {
                int argOffset = g_nextParamStackLocation;
            
                stream << "movq ";
                stream << (argOffset*8+16) << "(%rbp), ";
                IrOutputArg(g_tempReg, stream);
                stream << std::endl;

                stream << "movq ";
                IrOutputArg(g_tempReg, stream);
                stream << ", ";
                IrOutputArg(stmt.m_src0, stream);
                stream << std::endl;
                
                g_nextParamStackLocation++;
            }   
        }
        else
        {
            int argOffset = g_nextParamStackLocation;

            stream << "movl ";
            stream << (argOffset*8+16) << "(%rbp), ";
            IrOutputArg(g_tempReg, stream);
            stream << std::endl;
 
            stream << "movl ";
            IrOutputArg(g_tempReg, stream);
            stream << ", ";
            IrOutputArg(stmt.m_src0, stream);
            stream << std::endl;
            
            g_nextParamStackLocation++;
        }
        break;
        
    case IrOpcode::STRING:     // string label -> arg0 value -> arg1
        IrOutputLabel(stmt.m_src0, stream);
        
        stream << ":" << std::endl;
        stream << ".string ";
        
        IrOutputLabel(stmt.m_src1, stream);
        stream << std::endl;
        break;
        
    case IrOpcode::GLOBAL:
        stream << ".lcomm ";    // put global allocations in zero-initialized section (ie bss)
        IrOutputLabel(stmt.m_src0, stream);
        stream << "," << stmt.m_info << std::endl;
        break;
        
    default:
        break;
    }
    
}

} // namespace Decaf
