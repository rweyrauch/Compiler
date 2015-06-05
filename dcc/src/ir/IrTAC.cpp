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
#include <sstream>
#include <cassert>
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
    "GLOBAL",
    "DOUBLE",
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
        std::stringstream str;
        str << m_value.m_int;
        m_asString = str.str();
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

void IrTacArg::buildLabel(const std::string& label)
{
    m_usage = IrUsage::Label;
    m_isConstant = true;
    m_type = IrArgType::String;
    m_asString = label;
}

void IrTacArg::build(const std::string& literal)
{
    m_usage = IrUsage::Literal;
    m_isConstant = true;
    m_type = IrArgType::String;
    m_asString = literal;
}

void IrTacArg::build(double literal)
{
    m_usage = IrUsage::Literal;
    m_isConstant = true;
    m_type = IrArgType::Double;
    m_value.m_double = literal;
    std::stringstream str;
    str << m_value.m_int;
    m_asString = str.str();
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

std::ostream& operator<<(std::ostream& stream,const IrTacArg& arg)
{
    switch (arg.m_usage)
    {
    case IrUsage::Label:
        stream << "$" << arg.m_asString;
        break;
    case IrUsage::Global:
        stream << arg.m_asString << "(%rip)";
        break;
    case IrUsage::Identifier:
    case IrUsage::Argument:
        stream << "-" << arg.m_value.m_address+8 <<  "(%rbp)";       
        break;
    case IrUsage::Literal:
        stream << "$" << arg.m_asString;
        break;
    case IrUsage::Register:
        stream << g_registerNames[arg.m_value.m_int];
        break;
    default:
        // Unexpected.
        break;
    }
    return stream;
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

void IrGenMov(const IrTacArg& src, const IrTacArg& dst, std::ostream& stream)
{
    if (src.isMemory() && dst.isMemory())
    {
        if (src.isDouble() || dst.isDouble())
        {
            stream << "movsd " << src << ", " << g_tempDoubleReg << std::endl;     
            stream << "movsd " << g_tempDoubleReg << ", " << dst << std::endl;                  
        }
        else
        {
            stream << "movq " << src << ", " << g_tempReg << std::endl;     
            stream << "movq " << g_tempReg << ", " << dst << std::endl;      
        }
    }
    else
    {
        if (src.isDouble() || dst.isDouble())
        {
            if (src.isLiteral())
            {
                stream << "movabsq " << src << ", " << g_tempReg << std::endl;
                stream << "movq " << g_tempReg << ", " << dst << std::endl;
            }
            else if (src.isRegister() || dst.isRegister())
            {
                stream << "movsd " << src << ", " << dst << std::endl;				
            }
            else
            {
                stream << "movq " << src << ", " << dst << std::endl;
            }
        }
        else
        {
            stream << "movq " << src << ", " << dst << std::endl;
        }
    }
}

static int s_boundsCounter = 0;

void IrGenBoundsCheck(const IrTacArg& offset, int limit, int lineNo, std::ostream& stream)
{
    std::stringstream labelPass;
    labelPass << ".LB" << s_boundsCounter++;
    std::stringstream labelFail;
    labelFail << ".LB" << s_boundsCounter++;
    
    IrGenMov(offset, g_tempReg, stream); 
    
    stream << "cmpq $0," << g_tempReg << std::endl;
    stream << "jl " << labelFail.str() << std::endl;
    
    stream << "cmpq $" << limit << "," << g_tempReg << std::endl;
    stream << "jl " << labelPass.str() << std::endl;    

    stream << labelFail.str() << ":" << std::endl;    
    
    // puts(.BOUNDSMSG)
    stream << "mov $.BOUNDSMSG, %rdi" << std::endl;
    stream << "mov $.DCFFILE, %rsi" << std::endl;
    stream << "mov $" << lineNo << ", %rdx" << std::endl;
    stream << "call printf" << std::endl;
    
    // abort() 
    stream << "call abort" << std::endl;

    stream << labelPass.str() << ":" << std::endl;    
}

void IrGenLoad(const IrTacArg& baseAddr, const IrTacArg& offset, const IrTacArg& dst, int limit, int lineNo, std::ostream& stream)
{
    // add bounds check to offset argument, range in offset.m_info
    IrGenBoundsCheck(offset, limit, lineNo, stream); 
    
    // load offset value into rsi/esi register
    IrGenMov(offset, g_indexRegister, stream);
        
    stream << "movq ";
    
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
        stream << "(,%rsi,8), ";
    }
    stream << g_tempReg << std::endl;
    
    stream << "movq " << g_tempReg << ", " << dst << std::endl;      
}

void IrGenStore(const IrTacArg& src, const IrTacArg& baseAddr, const IrTacArg& offset, int limit, int lineNo, std::ostream& stream)
{
    // add bounds check to offset argument, range in offset.m_info
    IrGenBoundsCheck(offset, limit, lineNo, stream); 
            
    // Load source value into a register.
    stream << "movq " << src << ", " << g_tempReg << std::endl;

    // load offset value into rsi/esi register
    IrGenMov(offset, g_indexRegister, stream);

    stream << "movq " << g_tempReg << ",";
    
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
        stream << "(,%rsi,8)";
    }
    stream << std::endl;
}

void IrGenComparison(const IrTacStmt& stmt, std::ostream& stream)
{    
    // zero the temp output reg
    stream << "xor " << g_retReg << ", " << g_retReg << std::endl;
    
    if (stmt.m_src0.isDouble())
    {
		unsigned int compOp = 0;
		switch (stmt.m_opcode)
		{
		case IrOpcode::EQUAL:      // arg0 == arg1 -> arg2 (0 or 1)
			compOp = 0;
			break;
					
		case IrOpcode::NOTEQUAL:   // arg0 != arg1 -> arg2 (0 or 1)
			compOp = 4;
			break;
			
		case IrOpcode::LESS:       // arg0 < arg1 -> arg2 (0 or 1)
			compOp = 1;
			break;
			
		case IrOpcode::LESSEQUAL:  // arg0 <= arg1 -> arg2 (0 or 1)
			compOp = 2;
			break;
			
		case IrOpcode::GREATER:    // arg0 > arg1 -> arg2 (0 or 1)
			compOp = 6;
			break;
			
		case IrOpcode::GREATEREQUAL: // arg0 >= arg1 -> arg2 (0 or 1)
			compOp = 5;
			break;
			
		default:
			break;
		}
		
		// make sure the first arg is not an immediate nor a memory access
		IrGenMov(stmt.m_src0, g_tempDoubleReg, stream);
		stream << "cmpsd $" << compOp << ", " << stmt.m_src1 << ", " << g_tempDoubleReg << std::endl;  
	}
	else
	{
		// make sure the second arg is not an immediate nor a memory access
 		IrGenMov(stmt.m_src0, g_tempReg, stream);
		stream << "cmp " << stmt.m_src1 << ", " << g_tempReg << std::endl;  
    
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
		stream << g_retRegWord << std::endl;  
	}

    stream << "movzb " << g_retRegWord << ", " << g_retReg << std::endl;
    
    IrGenMov(g_retReg, stmt.m_dst, stream);      
}

static std::vector<IrTacArg> g_funcCallParams;

void IrGenParamPush(std::ostream& stream)
{
    std::vector<IrTacArg> extraParams;
    
    int nextDoubleRegister = 0;
    int nextNormalRegister = 0;

    for (auto it : g_funcCallParams)
    {
        if (it.isDouble())
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

    // Pop extra parameters off the param stack
    if (!extraParams.empty())
    {
        // align the stack 16-byte boundary
        if (extraParams.size() % 2 == 1)
            stream << "push $0" << std::endl;
        
        for (auto it = extraParams.rbegin(); it != extraParams.rend(); ++it)
        {
            stream << "push " << *it << std::endl;
        }
    }   
        
    g_funcCallParams.clear();
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
        IrGenLoad(stmt.m_src0, stmt.m_src1, stmt.m_dst, stmt.m_info, stmt.m_lineNo, stream);
        break;
        
    case IrOpcode::STORE:      // arg0 -> *[arg1 + arg2]
        IrGenStore(stmt.m_src0, stmt.m_src1, stmt.m_dst, stmt.m_info, stmt.m_lineNo, stream);
        break;
        
    case IrOpcode::ADD:        // arg0 + arg1 -> arg2
    case IrOpcode::SUB:        // arg0 - arg1 -> arg2
        if (stmt.m_dst.isDouble())
        {
            IrGenMov(stmt.m_src0, g_tempDoubleReg, stream);
            IrGenMov(stmt.m_src1, g_retDoubleReg, stream);
            
            stream << (stmt.m_opcode == IrOpcode::ADD ? "addsd " : "subsd ") << g_retDoubleReg << ", " << g_tempDoubleReg << std::endl;
        
            IrGenMov(g_tempDoubleReg, stmt.m_dst, stream);
        }
        else
        {
            IrGenMov(stmt.m_src0, g_tempReg, stream);
        
            stream << (stmt.m_opcode == IrOpcode::ADD ? "add " : "sub ") << stmt.m_src1 << ", " << g_tempReg << std::endl;
        
            IrGenMov(g_tempReg, stmt.m_dst, stream);
        }
        break;
        
    case IrOpcode::MUL:        // arg0 * arg1 -> arg2
    case IrOpcode::DIV:        // arg0 / arg1 -> arg2
    case IrOpcode::MOD:        // arg0 % arg1 -> arg2  
        if (stmt.m_dst.isDouble())
        {
            IrGenMov(stmt.m_src0, g_tempDoubleReg, stream);
            IrGenMov(stmt.m_src1, g_retDoubleReg, stream);

            stream << (stmt.m_opcode == IrOpcode::MUL ? "mulsd " : "divsd ") << g_retDoubleReg << ", " << g_tempDoubleReg << std::endl;
        
            IrGenMov(g_tempDoubleReg, stmt.m_dst, stream);
        }
        else
        {
            // zero the temp output reg
            stream << "xor " << g_outReg << ", " << g_outReg << std::endl;
            
            IrGenMov(stmt.m_src0, g_retReg, stream); // arg0 => %rax
            
            // make sure the second arg is not an immediate nor a memory access
            IrGenMov(stmt.m_src1, g_tempReg, stream);
            
            stream << (stmt.m_opcode == IrOpcode::MUL ? "imul " : "idiv ") << g_tempReg << std::endl;
            
            if (stmt.m_opcode == IrOpcode::MOD)
            {
                IrGenMov(g_outReg, stmt.m_dst, stream); // %rdx => arg2                       
            }
            else
            {
                IrGenMov(g_retReg, stmt.m_dst, stream); // %rax => arg2
            }
        }
        break;
        
    case IrOpcode::CALL:       // call arg0 arg1
        IrGenParamPush(stream);

        stream << "call " << stmt.m_src0.m_asString << std::endl;
        
        if (stmt.hasSrc1())
        {
            IrGenMov(g_retReg, stmt.m_src1, stream);
        }
        break;
        
    case IrOpcode::FBEGIN:     // begin function
        stream << ".global " << stmt.m_src0.m_asString << std::endl;
        stream << stmt.m_src0.m_asString << ":" << std::endl; 
        stream << "enter $" << stmt.m_info << ", $0" << std::endl;
        
        g_nextUsedDoubleParamReg = 0;
        g_nextUsedNormalParamReg = 0;
        g_nextParamStackLocation = 0;
        break;
        
    case IrOpcode::RETURN:     // return |arg0|
        if (stmt.hasSrc0())
        {
            if (stmt.m_src0.isDouble())
            {
                IrGenMov(stmt.m_src0, g_retDoubleReg, stream);
            }
            else
            {
                IrGenMov(stmt.m_src0, g_retReg, stream);
            }
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
    
        stream << (stmt.m_opcode == IrOpcode::AND ? "and " : "or ") << stmt.m_src0 << ", " << g_tempReg << std::endl;
        
        IrGenMov(g_tempReg, stmt.m_dst, stream);
        break;
        
    case IrOpcode::NOT:        // !arg1 -> arg2 (0 or 1)
        // make sure the arg is not an immediate 
        IrGenMov(stmt.m_src1, g_tempReg, stream);
        
        // 'not' is a bit-wise invert 
        stream << "not " << g_tempReg << std::endl;
        
        // mask all but first bit in result
        stream << "and $1," << g_tempReg << std::endl;
        
        IrGenMov(g_tempReg, stmt.m_dst, stream);
        break;
        
    case IrOpcode::LABEL:      // arg0:
        stream << stmt.m_src0.m_asString << ":" << std::endl;     
        break;
        
    case IrOpcode::JUMP:       // jump arg0
        stream << "jmp " << stmt.m_src0.m_asString << std::endl;
        break;
        
    case IrOpcode::IFZ:        // branch arg0 == 0 to arg1
        if (stmt.m_src0.m_usage != IrUsage::Literal)
        {    
            stream << "cmp $0, " << stmt.m_src0 << std::endl;           
        }
        else // literal
        {
            IrGenMov(stmt.m_src0, g_tempReg, stream);
            
            stream << "cmp $0, " << g_tempReg << std::endl;                       
        }
        stream << "jz " << stmt.m_src1.m_asString  << std::endl;
    break;
        
    case IrOpcode::IFNZ:        // jump if cmp != 0
        if (stmt.m_src0.m_usage != IrUsage::Literal)
        {    
            stream << "cmp $0, " << stmt.m_src0 << std::endl;
        }
        else
        {
            IrGenMov(stmt.m_src0, g_tempReg, stream);
            
            stream << "cmp $0, " << g_tempReg << std::endl;                       
        }
        
        stream << "jnz " << stmt.m_src1.m_asString << std::endl;
        break;
        
    case IrOpcode::PARAM:       // push arg0 -> stack
        g_funcCallParams.push_back(stmt.m_src0);
        break;
    
    case IrOpcode::GETPARAM:	// stack -> arg0
        {
            bool onStack = false;
            if (stmt.m_src0.isDouble())
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
            
                stream << "movq " << (argOffset*8+16) << "(%rbp), " << g_tempReg << std::endl;

                stream << "movq " << g_tempReg << ", " << stmt.m_src0 << std::endl;
                
                g_nextParamStackLocation++;
            }   
        }
        break;
        
    case IrOpcode::STRING:     // string label -> arg0 value -> arg1
        stream << stmt.m_src0.m_asString << ":" << std::endl;
        stream << ".string \"" << stmt.m_src1.m_asString << "\"" << std::endl;
        break;
        
    case IrOpcode::GLOBAL:
        // put global allocations in zero-initialized section (ie bss)
        stream << ".lcomm " << stmt.m_src0.m_asString << "," << stmt.m_info << std::endl;
        break;
        
    case IrOpcode::DOUBLE:  // double label -> arg0 value -> arg1
        stream << stmt.m_src0.m_asString << ":" << std::endl;
        stream << ".double " << stmt.m_src1.m_asString << std::endl;
        
    default:
        break;
    }
    
}

} // namespace Decaf
