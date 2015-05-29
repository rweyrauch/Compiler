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
#include "IrBasicBlock.h"
#include "IrTAC.h"
#include "IrIdentifier.h"
#include "IrLiteral.h"

namespace Decaf
{

bool isBinaryOp(IrOpcode opcode)
{
    switch (opcode)
    {
        case IrOpcode::ADD:
        case IrOpcode::SUB:
        case IrOpcode::MUL:
        case IrOpcode::DIV:
        case IrOpcode::MOD:
            return true;
        default:
            break;
    }
    return false;
}

bool isMoveOp(IrOpcode opcode)
{
	return (opcode == IrOpcode::MOV);
}

int IrBasicBlock::getValueNumber(const std::string& ident)
{  
	int valueNumber = -1;
	auto ip = m_variable_value_map.find(ident);
	if (ip == m_variable_value_map.end())
	{
		valueNumber = m_next_value_number++;
		m_variable_value_map[ident] = valueNumber;
	}
	else
	{
		valueNumber = ip->second;
	}
	return valueNumber;
}

bool IrBasicBlock::commonSubexpressionElimination()
{
    m_variable_value_map.clear();
    m_expression_value_map.clear();
    
    // Statement of form: D = L op R
    for (auto it : m_statements)
    {
        if (!isBinaryOp(it.m_opcode) && !isMoveOp(it.m_opcode))
            continue;
        
        // Get/create the value numbers of D, L and R    
        it.m_src0.m_valueNumber = getValueNumber(it.m_src0.m_asString);
         
        if (it.hasSrc1())
        {
			it.m_src1.m_valueNumber = getValueNumber(it.m_src1.m_asString);
        }
        else
        {
			it.m_src1.m_valueNumber = -1;
		}
		
        it.m_dst.m_valueNumber = getValueNumber(it.m_dst.m_asString);
        
        // Create key from opcode, L and R.
        Key keyExpr(it.m_src0.m_valueNumber, it.m_opcode, it.m_src1.m_valueNumber);
                
        auto mip = m_expression_value_map.find(keyExpr);
        if (mip == m_expression_value_map.end())
        {
            int value = m_next_value_number++;
            m_expression_value_map[keyExpr] = value;
            
            // Record new value for D.
            m_variable_value_map[it.m_dst.m_asString] = value;
        }
        else
        {
            // Replace exist value for D with value from expression.
            m_variable_value_map[it.m_dst.m_asString] = mip->second;
        }
    }
    
    std::cout << "Variable-Value Map" << std::endl;
    for (auto it : m_variable_value_map)
    {
        std::cout << "[" << it.first << "] = " << it.second << std::endl;
    }
    std::cout << "Expression-Value Map" << std::endl;
    for (auto it : m_expression_value_map)
    {
        std::cout << "[" << it.first.m_left << ", " << IrOpcodeToString(it.first.m_op) << ", " << it.first.m_right << "] = " << it.second << std::endl;
    }
        
    for (auto it : m_statements)
    {
        IrPrintTac(it, std::cout);
    }

    return true;
}

bool IrBasicBlock::codegen(IrTraversalContext* ctx)
{
    return false;
}

void IrBasicBlock::print(std::ostream& stream)
{
    if (!m_statements.empty())
    {
        stream << "----Begin----" << std::endl;
        
        for (auto it : m_statements)
        {
            IrPrintTac(it, stream);
        }
        
        stream << "----End----" << std::endl;
    }
}

} // namespace Decaf

