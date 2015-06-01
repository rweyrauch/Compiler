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
#include <sstream>
#include <list>
#include <algorithm>
#include <cassert>
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

bool isLogicOp(IrOpcode opcode)
{
    switch (opcode)
    {
        case IrOpcode::OR:
        case IrOpcode::AND:
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

bool isTempIdentifier(const IrTacArg& arg)
{
    if ((arg.m_usage == IrUsage::Identifier) && (arg.m_asString.find_first_of(".LC") != std::string::npos))
        return true;
    return false;
}

bool isIntegerZero(const IrTacArg& arg)
{
    return (arg.m_usage == IrUsage::Literal && arg.m_type == IrArgType::Integer && arg.m_value.m_int == 0);  
}
bool isIntegerOne(const IrTacArg& arg)
{
    return (arg.m_usage == IrUsage::Literal && arg.m_type == IrArgType::Integer && arg.m_value.m_int == 1);  
}
bool isTrue(const IrTacArg& arg)
{
    return (arg.m_usage == IrUsage::Literal && arg.m_type == IrArgType::Boolean && arg.m_value.m_int == 1);  
}
bool isFalse(const IrTacArg& arg)
{
    return (arg.m_usage == IrUsage::Literal && arg.m_type == IrArgType::Boolean && arg.m_value.m_int == 0);  
}

int evaluateConstIntExpression(IrOpcode opcode, int src0, int src1)
{
    int value = 0;
    
    switch (opcode)
    {
        case IrOpcode::ADD:
            value = src0 + src1;
            break;
        case IrOpcode::SUB:
            value = src0 - src1;
            break;
        case IrOpcode::MUL:
            value = src0 * src1;
            break;
        case IrOpcode::DIV:
            value = src0 / src1;
            break;
        case IrOpcode::MOD:
            value = src0 % src1;
            break;    
        case IrOpcode::MOV:
            value = src0;
            break;
        default:
            assert(false);
            break;
    }
    return value;
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

void IrBasicBlock::optimize(IrBasicBlockOpts which)
{
    if (which & BBOPTS_CONSTANT_PROP)
        constantPropagation();  
    if (which & BBOPTS_ALGEBRAIC_SIMP)
        algebraicSimplification();
    if (which & BBOPTS_COMMON_SUBEXPR_ELIM)
        commonSubexpressionElimination();
    if (which & BBOPTS_COPY_PROP)
        copyPropagation();
    if (which & BBOPTS_DEAD_CODE_ELIM)
        deadCodeElimination(); 
}

void IrBasicBlock::constantPropagation()
{
    std::map<std::string, int> integerConstants;
    
    for (auto it = m_statements.begin(); it != m_statements.end(); ++it)
    {
        if (!isBinaryOp(it->m_opcode) && !isMoveOp(it->m_opcode) && !isLogicOp(it->m_opcode))
        {
            continue;
        }
        
        // check for expression with constant results
        bool src0IsConstant = (it->m_src0.m_usage == IrUsage::Literal);
        int src0 = it->m_src0.m_value.m_int;
        if (it->m_src0.m_usage == IrUsage::Identifier && it->m_src0.m_type == IrArgType::Integer)
        {
            // lookup the identifier in the constant table
            auto cip = integerConstants.find(it->m_src0.m_asString);
            if (cip != integerConstants.end())
            {
                src0IsConstant = true;
                src0 = cip->second;
            }
        }
        
        bool src1IsConstant = (it->hasSrc1() && it->m_src1.m_usage == IrUsage::Literal);
        int src1 = it->m_src1.m_value.m_int;
        if (!it->hasSrc1())
        {
            src1IsConstant = true;
            src1 = 0;
        }
        else if (it->m_src1.m_usage == IrUsage::Identifier && it->m_src1.m_type == IrArgType::Integer)
        {
            // lookup the identifier in the constant table
            auto cip = integerConstants.find(it->m_src1.m_asString);
            if (cip != integerConstants.end())
            {
                src1IsConstant = true;
                src1 = cip->second;
            }            
        }
        
        bool dstIsConstant = src0IsConstant && src1IsConstant;

        if (dstIsConstant)
        {
            int value = evaluateConstIntExpression(it->m_opcode, src0, src1);
            std::stringstream str;
            str << value;
            
            integerConstants[it->m_dst.m_asString] = value;
            
            it->m_opcode = IrOpcode::MOV;
            it->m_src0.m_usage = IrUsage::Literal;
            it->m_src0.m_type = IrArgType::Integer;
            it->m_src0.m_value.m_int = value;
            it->m_src0.m_asString = str.str();
            it->m_src1.m_usage = IrUsage::Unused;
        }
        else
        {
            // remove the dst from the constant table if present
            auto cip = integerConstants.find(it->m_dst.m_asString);
            if (cip != integerConstants.end())
            {
                integerConstants.erase(cip);
            }
        }
    }   
}

void IrBasicBlock::algebraicSimplification()
{
    for (auto it = m_statements.begin(); it != m_statements.end(); ++it)
    {
        if (!isBinaryOp(it->m_opcode) && !isMoveOp(it->m_opcode) && !isLogicOp(it->m_opcode))
        {
            continue;
        }
        
        // Add zero, arg0 + 0 or 0 + arg1
        if (it->m_opcode == IrOpcode::ADD)
        {
            if (isIntegerZero(it->m_src0))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src0 = it->m_src1;
                it->m_src1.m_usage = IrUsage::Unused;                
            }
            else if (isIntegerZero(it->m_src1))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src1.m_usage = IrUsage::Unused;
           }
        }
        // Subtract zero, arg0 - 0
        // TODO: Implement 0 - arg1
        if (it->m_opcode == IrOpcode::SUB)
        {
            if (isIntegerZero(it->m_src1))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src1.m_usage = IrUsage::Unused;
           }            
        }
        
        // Multiply by 1, arg0 * 1 or 1 * arg1
        if (it->m_opcode == IrOpcode::MUL)
        {
            if (isIntegerOne(it->m_src0))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src0 = it->m_src1;
                it->m_src1.m_usage = IrUsage::Unused;
            }
            else if (isIntegerOne(it->m_src1))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src1.m_usage = IrUsage::Unused;                
            }
        }
        
        // Multiply by zero
        if (it->m_opcode == IrOpcode::MUL)
        {
            if (isIntegerZero(it->m_src0))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src1.m_usage = IrUsage::Unused;
            }
            else if (isIntegerZero(it->m_src1))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src0 = it->m_src1;
                it->m_src1.m_usage = IrUsage::Unused;                
            }
        }
        
        // Or with true
        if (it->m_opcode == IrOpcode::OR)
        {
            if (isTrue(it->m_src0))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src1.m_usage = IrUsage::Unused;
            }
            else if (isTrue(it->m_src1))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src0 = it->m_src1;
                it->m_src1.m_usage = IrUsage::Unused;                
            }
        }
            
        // And with false
        if (it->m_opcode == IrOpcode::AND)
        {
            if (isFalse(it->m_src0))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src1.m_usage = IrUsage::Unused;
            }
            else if (isFalse(it->m_src1))
            {
                it->m_opcode = IrOpcode::MOV;
                it->m_src0 = it->m_src1;
                it->m_src1.m_usage = IrUsage::Unused;                
            }
        }
    }
}

void IrBasicBlock::commonSubexpressionElimination()
{
    m_variable_value_map.clear();
    m_expression_value_map.clear();
    m_expression_temp_map.clear();
    
    // Statement of form: D = L op R
    for (auto it = m_statements.begin(); it != m_statements.end(); ++it)
    {
        if (!isBinaryOp(it->m_opcode) && !isMoveOp(it->m_opcode) && !isLogicOp(it->m_opcode))
        {
            continue;
        }
        
        // Get/create the value numbers of D, L and R    
        it->m_src0.m_valueNumber = getValueNumber(it->m_src0.m_asString);
        it->m_src0.m_isConstant = (it->m_src0.m_usage == IrUsage::Literal);
        
        if (it->hasSrc1())
        {
            it->m_src1.m_valueNumber = getValueNumber(it->m_src1.m_asString);
            it->m_src1.m_isConstant = (it->m_src1.m_usage == IrUsage::Literal);
        }
        else
        {
            it->m_src1.m_valueNumber = -1;
        }
        
        it->m_dst.m_valueNumber = getValueNumber(it->m_dst.m_asString);
        assert(it->m_dst.m_usage != IrUsage::Literal);
        it->m_dst.m_isConstant = false;
        
        // Create key from opcode, L and R.
        Key keyExpr(it->m_src0.m_valueNumber, it->m_opcode, it->m_src1.m_valueNumber);
           
        if (isTempIdentifier(it->m_dst))
        {
            auto tip = m_expression_temp_map.find(keyExpr);
            if (tip == m_expression_temp_map.end())
            {
                m_expression_temp_map[keyExpr] = it->m_dst;
            }
        }        
        
        auto mip = m_expression_value_map.find(keyExpr);
        if (mip == m_expression_value_map.end())
        {
            int value = m_next_value_number++;
            m_expression_value_map[keyExpr] = value;
            
            // Record new value for D.
            m_variable_value_map[it->m_dst.m_asString] = value;
            it->m_dst.m_valueNumber = value;
        }
        else
        {
            // Replace exist value for D with value from expression.
            m_variable_value_map[it->m_dst.m_asString] = mip->second;
            it->m_dst.m_valueNumber = mip->second;
            
            auto tip = m_expression_temp_map.find(keyExpr);
            if (tip != m_expression_temp_map.end())
            {
                // Replace the current expression statement with a MOV.
                it->m_opcode = IrOpcode::MOV;
                it->m_src0 = tip->second;
                it->m_src1.m_usage = IrUsage::Unused;
            }
        }
    }
    
    if (m_verbose)
    {
        if (!m_variable_value_map.empty())
            std::cout << "Variable-Value Map" << std::endl;
        for (auto it : m_variable_value_map)
        {
            std::cout << "[" << it.first << "] = " << it.second << std::endl;
        }
        if (!m_expression_value_map.empty())
            std::cout << "Expression-Value Map" << std::endl;
        for (auto it : m_expression_value_map)
        {
            std::cout << "[" << it.first.m_left << ", " << IrOpcodeToString(it.first.m_op) << ", " << it.first.m_right << "] = " << it.second << std::endl;
        }
        if (!m_expression_temp_map.empty())
            std::cout << "Expression-Temp Map" << std::endl;
        for (auto it : m_expression_temp_map)
        {
            std::cout << "[" << it.first.m_left << ", " << IrOpcodeToString(it.first.m_op) << ", " << it.first.m_right << "] = " << it.second.m_asString << std::endl;
        }
    }
}

void IrBasicBlock::copyPropagation()
{
    std::map<std::string, IrTacArg> temp_to_var_map;
    std::map<std::string, std::string> var_to_temp_map;
    
    for (auto it = m_statements.begin(); it != m_statements.end(); ++it)
    {
        if (!isBinaryOp(it->m_opcode) && !isMoveOp(it->m_opcode) && !isLogicOp(it->m_opcode))
        {
            continue;
        }

        if (it->hasSrc0() && isTempIdentifier(it->m_src0))
        {
            auto tip = temp_to_var_map.find(it->m_src0.m_asString);
            if (tip != temp_to_var_map.end())
            {
                it->m_src0 = tip->second;
            }
        }
		if (it->hasSrc1() && isTempIdentifier(it->m_src1))
		{
            auto tip = temp_to_var_map.find(it->m_src1.m_asString);
            if (tip != temp_to_var_map.end())
            {
                it->m_src1 = tip->second;
            }			
		}
        if (isTempIdentifier(it->m_dst))
        {
			if (isMoveOp(it->m_opcode))
			{
				temp_to_var_map[it->m_dst.m_asString] = it->m_src0;
				var_to_temp_map[it->m_src0.m_asString] = it->m_dst.m_asString;
			}
        }
        else
        {
			if (isMoveOp(it->m_opcode))
			{
				auto vip = var_to_temp_map.find(it->m_dst.m_asString);
				if (vip != var_to_temp_map.end())
				{
					auto tip = temp_to_var_map.find(vip->second);
					if (tip != temp_to_var_map.end())
					{
						temp_to_var_map.erase(tip);
					}
					var_to_temp_map.erase(vip);
				}
			}
		}
    }
}
    
void IrBasicBlock::deadCodeElimination()
{
    std::vector<std::string> needed_var_set;
    
    for (auto it = m_statements.rbegin(); it != m_statements.rend(); ++it)
    {
        if (!isMoveOp(it->m_opcode))
        {
            continue;
        }
        
        if (isTempIdentifier(it->m_dst))
        {
            auto vip = std::find(needed_var_set.begin(), needed_var_set.end(), it->m_src0.m_asString);
            if (vip != needed_var_set.end())
            {
                // copy not needed
                it->m_opcode = IrOpcode::NOOP;
                it->m_src0.m_usage = IrUsage::Unused;
                it->m_src1.m_usage = IrUsage::Unused;
                it->m_dst.m_usage = IrUsage::Unused;
            }
         }
        else
        {
            needed_var_set.push_back(it->m_src0.m_asString);
        }
    }
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

