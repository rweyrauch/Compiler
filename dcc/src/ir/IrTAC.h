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
#include <memory>
#include <string>
#include <iostream>
#include "IrBase.h"

namespace Decaf
{
class IrIdentifier;
class IrLiteral;
class IrIntegerLiteral;
class IrBooleanLiteral;
class IrStringLiteral;
class IrDoubleLiteral;

enum class IrOpcode : int
{
    NOOP = 0,
    MOV,        // arg0 -> dst
    LOAD,       // *[arg0 + arg1] -> dst
    STORE,      // arg0 -> *[arg1 + dst]
    ADD,        // arg0 + arg1 -> dst
    SUB,        // arg0 - arg1 -> dst
    MUL,        // arg0 * arg1 -> dst
    DIV,        // arg0 / arg1 -> dst
    MOD,        // arg0 % arg1 -> dst
    CALL,       // call arg0 arg1
    FBEGIN,     // begin function
    RETURN,     // return |arg0|
    EQUAL,      // arg0 == arg1 -> dst (0 or 1)
    NOTEQUAL,   // arg0 != arg1 -> dst (0 or 1)
    LESS,       // arg0 < arg1 -> dst (0 or 1)
    LESSEQUAL,  // arg0 <= arg1 -> dst (0 or 1)
    GREATER,    // arg0 > arg1 -> dst (0 or 1)
    GREATEREQUAL, // arg0 >= arg1 -> dst (0 or 1)
    AND,        // arg0 && arg1 -> dst (0 or 1)
    OR,         // arg0 || arg1 -> dst (0 or 1)
    NOT,        // !arg0 -> dst (0 or 1)
    LABEL,      // arg0:
    JUMP,       // jump arg0
    IFZ,        // branch arg0 == 0 to arg1
    IFNZ,       // branch arg0 != 0 to arg1
    PARAM,      // function call param, arg0 => ident info => argument number
    GETPARAM,	// get value from param, arg0 => ident info => argument number
    STRING,     // string label -> arg0 value -> arg1
    GLOBAL,     // global arg0
    DOUBLE,     // double label -> arg0 value -> arg1
    
    NUM_OPCODES
};

const std::string& IrOpcodeToString(IrOpcode opcode);

// Tac src/dst type:
//   m_use: Label, Identifier, Literal, Global
//   m_isConstant
//   m_type: Integer, Boolean, Double, String
//   m_value { union }
enum class IrUsage : unsigned char
{
    Unused,
    Label,
    Identifier,
    Global,
    Literal,
    Argument,
    Register
};
enum class IrArgType : unsigned char
{
    Integer,
    Boolean,
    Double,
    String
};

struct IrTacArg
{
    IrTacArg() :
        m_usage(IrUsage::Unused),
        m_type(IrArgType::String),
        m_asString(""),
        m_isConstant(false),
        m_valueNumber(0)
    {
        m_value.m_int = 0;
    }        
   
    void build(const IrIdentifier* ident);
    void build(const IrLiteral* literal);
    void build(const IrIntegerLiteral* literal);
    void build(const IrDoubleLiteral* literal);
    void build(const IrBooleanLiteral* literal);
    void build(const IrStringLiteral* literal);
    void buildLabel(const std::string& label);
    void build(const std::string& literal);
    void build(double literal);
    
    bool isLiteral() const { return (m_usage == IrUsage::Literal); }
    bool isMemory() const { return (m_usage == IrUsage::Identifier) || (m_usage == IrUsage::Global); }
    bool isRegister() const { return (m_usage == IrUsage::Register); }
    bool isDouble() const { return (m_type == IrArgType::Double); }
    
    IrUsage m_usage;
    IrArgType m_type;
    union {
        long int m_int;
        double m_double;
        std::ptrdiff_t m_address;
    } m_value;
    std::string m_asString;
    
    bool m_isConstant;
    int m_valueNumber;
};

struct IrTacStmt
{
    IrTacStmt() :
	m_opcode(IrOpcode::NOOP),
	m_src0(),
	m_src1(),
	m_dst(),
	m_info(0),
	m_lineNo(0)
    {}
    IrTacStmt(IrOpcode opcode, int lineNo = 0) :
        m_opcode(opcode),
        m_src0(),
        m_src1(),
        m_dst(),
        m_info(0),
        m_lineNo(lineNo)
    {}
        
    IrOpcode m_opcode;
    IrTacArg m_src0, m_src1, m_dst;
    int m_info;
    int m_lineNo;
    bool hasSrc0() const;
    bool hasSrc1() const;
    bool hasDst() const;
};

void IrPrintTac(const IrTacStmt& stmt, std::ostream& stream = std::cout);

void IrTacGenCode(const IrTacStmt& stmt, std::ostream& stream = std::cout);

struct Key
{
    Key(int left, IrOpcode opcode, int right) :
        m_left(left),
        m_op(opcode),
        m_right(right) {}
    Key() :
        m_left(0),
        m_op(IrOpcode::NOOP),
        m_right(0) {}
        
    int m_left;
    IrOpcode m_op;
    int m_right;
    
    bool operator<(const Key& lhs) const
    {
        if (m_left < lhs.m_left)
            return true;
        else if ((int)m_op < (int)lhs.m_op)
            return true;
        else if (m_right < lhs.m_right)
            return true;
        return false;
    }
    bool operator==(const Key& lhs) const
    {
        if ((m_left == lhs.m_left) &&
            (m_op == lhs.m_op) &&
            (m_right == lhs.m_right))
            return true;
        return false;
    }
};

struct KeyHasher
{
    size_t operator()(const Key& key) const
    {
        return ((std::hash<int>()(key.m_left) ^
                (std::hash<int>()((int)key.m_op) << 1)) >> 1) ^
                (std::hash<int>()(key.m_right) << 1);
    }
};

bool isBinaryOp(IrOpcode opcode);
bool isLogicOp(IrOpcode opcode);
bool isMoveOp(IrOpcode opcode);
bool isComparisonOp(IrOpcode opcode);
bool isTempIdentifier(const IrTacArg& arg);
bool isIntLiteral(const IrTacArg& arg);
bool isDoubleLiteral(const IrTacArg& arg);
bool isBoolLiteral(const IrTacArg& arg);
bool isIntegerZero(const IrTacArg& arg);
bool isIntegerOne(const IrTacArg& arg);
bool isDoubleZero(const IrTacArg& arg);
bool isDoubleOne(const IrTacArg& arg);
bool isTrue(const IrTacArg& arg);
bool isFalse(const IrTacArg& arg);

} // namespace Decaf
