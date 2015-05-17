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
#include <cstddef>
#include <string>
#include <vector>

namespace Decaf
{

enum class IrType : int
{
    Unknown = 0,
    Void,
    Integer,
    Boolean,
    Character,
    String,
    Double,
    
    NUM_IR_TYPES
};
bool IsNumeric(IrType type);
bool IsComparable(IrType type);

enum class IrBinaryOperator : int
{
    Add = 0,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    
    NUM_IR_BINARY_OPERATORS
};
    
 
enum class IrBooleanOperator : int
{
    Equal = 0,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    LogicalAnd,
    LogicalOr,
    Not,
    
    NUM_IR_BOOLEAN_OPERATORS
};

enum class IrAssignmentOperator : int
{
    Assign = 0,
    IncrementAssign,
    DecrementAssign,
    
    NUM_IR_ASSIGNMENT_OPERATORS
};

enum class IrMemLocation : int
{
    Local,
    Global,
    
    NUM_MEM_LOCATIONS
};

const std::string& IrTypeToString(IrType type);
const std::string& IrBinaryOperatorToString(IrBinaryOperator op);
const std::string& IrBooleanOperatorToString(IrBooleanOperator op);
const std::string& IrAssignmentOperatorToString(IrAssignmentOperator op);
const std::string& IrMemLocationToString(IrMemLocation loc);

#define IRPRINT_INDENT(depth) { for (int d = 0; d < (int)(depth); d++) std::cout << "  "; }

struct SVariableSymbol
{
    std::string m_name;
    IrType m_type;
    IrMemLocation m_where;
    size_t m_count; // >1 for arrays
    ptrdiff_t m_addr;  // offset from base of stack in bytes
};
  
struct SMethodSymbol
{
    std::string m_name;
    IrType m_type;
    std::vector<SVariableSymbol> m_arguments;
};

struct SStringSymbol
{
    std::string m_name;
    std::string m_value;
};

struct SDoubleSymbol
{
    std::string m_name;
    double m_value;
};

} // namespace Decaf
