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
    m_expression_temp_map.clear();
    
    std::vector<IrTacStmt> optStatements;
    
    // Statement of form: D = L op R
    for (auto it : m_statements)
    {
        if (!isBinaryOp(it.m_opcode) && !isMoveOp(it.m_opcode))
        {
            optStatements.push_back(it);
            continue;
        }
        
        // Get/create the value numbers of D, L and R    
        it.m_src0.m_valueNumber = getValueNumber(it.m_src0.m_asString);
        it.m_src0.m_isConstant = (it.m_src0.m_usage == IrUsage::Literal);
        
        if (it.hasSrc1())
        {
            it.m_src1.m_valueNumber = getValueNumber(it.m_src1.m_asString);
            it.m_src1.m_isConstant = (it.m_src1.m_usage == IrUsage::Literal);
        }
        else
        {
            it.m_src1.m_valueNumber = -1;
        }
        
        it.m_dst.m_valueNumber = getValueNumber(it.m_dst.m_asString);
        assert(it.m_dst.m_usage != IrUsage::Literal);
        it.m_dst.m_isConstant = false;
        
        // Create key from opcode, L and R.
        Key keyExpr(it.m_src0.m_valueNumber, it.m_opcode, it.m_src1.m_valueNumber);
           
        if (isTempIdentifier(it.m_dst))
        {
            auto tip = m_expression_temp_map.find(keyExpr);
            if (tip == m_expression_temp_map.end())
            {
                m_expression_temp_map[keyExpr] = it.m_dst;
            }
        }        
        
        auto mip = m_expression_value_map.find(keyExpr);
        if (mip == m_expression_value_map.end())
        {
            int value = m_next_value_number++;
            m_expression_value_map[keyExpr] = value;
            
            // Record new value for D.
            m_variable_value_map[it.m_dst.m_asString] = value;
            it.m_dst.m_valueNumber = value;
        }
        else
        {
            // Replace exist value for D with value from expression.
            m_variable_value_map[it.m_dst.m_asString] = mip->second;
            it.m_dst.m_valueNumber = mip->second;
            
            auto tip = m_expression_temp_map.find(keyExpr);
            if (tip != m_expression_temp_map.end())
            {
                // Replace the current expression statement with a MOV.
                it.m_opcode = IrOpcode::MOV;
                it.m_src0 = tip->second;
                it.m_src1.m_usage = IrUsage::Unused;
            }
        }
        optStatements.push_back(it);        
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
        
        std::cout << "Original statements: " << std::endl;
        for (auto it : m_statements)
        {
            IrPrintTac(it, std::cout);
        }
        std::cout << "Optimized statements: " << std::endl;
        for (auto it : optStatements)
        {
            IrPrintTac(it, std::cout);
        }
    }
    
    m_statements = optStatements;
    
    return true;
}

bool IrBasicBlock::copyPropagation()
{
    std::vector<IrTacStmt> optStatements;
    
    std::map<std::string, IrTacArg> temp_to_var_map;
    std::map<std::string, std::string> var_to_temp_map;
    
    for (auto it : m_statements)
    {
        if (!isMoveOp(it.m_opcode))
        {
            optStatements.push_back(it);
            continue;
        }

        if (isTempIdentifier(it.m_dst))
        {
            temp_to_var_map[it.m_dst.m_asString] = it.m_src0;
            var_to_temp_map[it.m_src0.m_asString] = it.m_dst.m_asString;
        }
        else if (isTempIdentifier(it.m_src0))
        {
            auto tip = temp_to_var_map.find(it.m_src0.m_asString);
            if (tip != temp_to_var_map.end())
            {
                it.m_src0 = tip->second;
            }
        }
        
        optStatements.push_back(it);
    }
 
    if (m_verbose)
    {
        std::cout << "Original statements: " << std::endl;
        for (auto it : m_statements)
        {
            IrPrintTac(it, std::cout);
        }
        std::cout << "Optimized statements: " << std::endl;
        for (auto it : optStatements)
        {
            IrPrintTac(it, std::cout);
        }
    }
    
    m_statements = optStatements;
    
    return true;
}
    
bool IrBasicBlock::deadCodeElimination()
{
    std::vector<std::string> needed_var_set;
    
    std::list<IrTacStmt> optStatements;
    
    for (auto it = m_statements.rbegin(); it != m_statements.rend(); ++it)
    {
        if (!isMoveOp(it->m_opcode))
        {
            optStatements.push_front(*it);
            continue;
        }
        
        if (isTempIdentifier(it->m_dst))
        {
            auto vip = std::find(needed_var_set.begin(), needed_var_set.end(), it->m_src0.m_asString);
            if (vip != needed_var_set.end())
            {
                // copy not needed
            }
            else
            {
                optStatements.push_front(*it);
            }
        }
        else
        {
            needed_var_set.push_back(it->m_src0.m_asString);
            optStatements.push_front(*it);
        }
    }
 
    if (m_verbose)
    {
        std::cout << "Original statements: " << std::endl;
        for (auto it : m_statements)
        {
            IrPrintTac(it, std::cout);
        }
        std::cout << "Optimized statements: " << std::endl;
        for (auto it : optStatements)
        {
            IrPrintTac(it, std::cout);
        }
    }

    m_statements.clear();
    for (auto it : optStatements)
    {
        m_statements.push_back(it);
    }
        
    return true;
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

