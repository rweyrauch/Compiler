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

#include "IrCommon.h"

namespace Decaf
{
    
const std::string gIrTypeStrings[(int)IrType::NUM_IR_TYPES] =
{
    std::string("Unknown"),
    std::string("Void"),
    std::string("Integer"),
    std::string("Boolean"),
    std::string("Character"),
    std::string("String"),
    std::string("Double")
};
static_assert(sizeof(gIrTypeStrings)/sizeof(std::string) == (size_t)IrType::NUM_IR_TYPES, "Unexpected number of IrType strings.");

bool IsNumeric(IrType type)
{
    return (type == IrType::Integer || type == IrType::Double);
}

bool IsComparable(IrType type)
{
    return (type == IrType::Integer || type == IrType::Double || type == IrType::Boolean);
}

const std::string gIrBinaryOperatorStrings[(int)IrBinaryOperator::NUM_IR_BINARY_OPERATORS] =
{
    std::string("Add"),
    std::string("Subtract"),
    std::string("Multiply"),
    std::string("Divide"),
    std::string("Modulo")
};
static_assert(sizeof(gIrBinaryOperatorStrings)/sizeof(std::string) == (size_t)IrBinaryOperator::NUM_IR_BINARY_OPERATORS, "Unexpected number of IrBinaryOperator strings.");
    

const std::string gIrBooleanOperatorStrings[(int)IrBooleanOperator::NUM_IR_BOOLEAN_OPERATORS] =
{
    std::string("Equal"),
    std::string("NotEqual"),
    std::string("Less"),
    std::string("LessEqual"),
    std::string("Greater"),
    std::string("GreaterEqual"),
    std::string("LogicalAnd"),
    std::string("LogicalOr"),
    std::string("Not")
};
static_assert(sizeof(gIrBooleanOperatorStrings)/sizeof(std::string) == (size_t)IrBooleanOperator::NUM_IR_BOOLEAN_OPERATORS, "Unexpected number of IrBooleanOperator strings.");

const std::string gIrAssignmentOperatorStrings[(int)IrAssignmentOperator::NUM_IR_ASSIGNMENT_OPERATORS] =
{
    std::string("Assign"),
    std::string("IncrementAssign"),
    std::string("DecrementAssign")
};
static_assert(sizeof(gIrAssignmentOperatorStrings)/sizeof(std::string) == (size_t)IrAssignmentOperator::NUM_IR_ASSIGNMENT_OPERATORS, "Unexpected number of IrAssignmentOperator strings.");

const std::string gIrMemLocationStrings[(int)IrMemLocation::NUM_MEM_LOCATIONS] =
{
    std::string("Local"),
    std::string("Global")
};
static_assert(sizeof(gIrMemLocationStrings)/sizeof(std::string) == (size_t)IrMemLocation::NUM_MEM_LOCATIONS, "Unexpected number or IrMemLocation strings.");

const std::string& IrTypeToString(IrType type)
{
    return gIrTypeStrings[(int)type];
}

const std::string& IrBinaryOperatorToString(IrBinaryOperator op)
{
    return gIrBinaryOperatorStrings[(int)op];
}

const std::string& IrBooleanOperatorToString(IrBooleanOperator op)
{
    return gIrBooleanOperatorStrings[(int)op];
}

const std::string& IrAssignmentOperatorToString(IrAssignmentOperator op)
{
    return gIrAssignmentOperatorStrings[(int)op];
}

const std::string& IrMemLocationToString(IrMemLocation loc)
{
    return gIrMemLocationStrings[(int)loc];
}

} // namespace Decaf