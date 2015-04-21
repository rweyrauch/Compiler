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
        case IrOpcode::MOV:
            return true;
        default:
            break;
    }
    return false;
}

bool IrBasicBlock::commonSubexpressionElimination()
{
    m_value_numbering_map.clear();
        
    // Statement of form: T <- L op R
    for (auto it : m_statements)
    {
        if (!isBinaryOp(it.m_opcode))
            continue;
        
        // Get/create the value numbers of T, L and R      
        Key keyL(it.m_src0->asString(), IrOpcode::NOOP, "");
        auto lip = m_value_numbering_map.find(keyL);
        if (lip == m_value_numbering_map.end())
        {
            m_value_numbering_map[keyL] = m_next_value_number++;
        }
        
        if (it.m_src1 != nullptr)
        {
            Key keyR(it.m_src1->asString(), IrOpcode::NOOP, "");
            auto rip = m_value_numbering_map.find(keyR);
            if (rip == m_value_numbering_map.end())
            {
                m_value_numbering_map[keyR] = m_next_value_number++;
            }
        }
        
        Key keyT(it.m_dst->asString(), IrOpcode::NOOP, "");
        auto tip = m_value_numbering_map.find(keyT);
        if (tip == m_value_numbering_map.end())
        {
            m_value_numbering_map[keyT] = m_next_value_number++;
        }
        
        // Create key from op, L and R.
        Key keyExpr(it.m_src0->asString(), it.m_opcode, (it.m_src1 != nullptr) ? it.m_src1->asString() : "");
                
        auto mip = m_value_numbering_map.find(keyExpr);
        if (mip != m_value_numbering_map.end())
        {
            m_value_numbering_map[keyT] = mip->second;
        }
        else
        {
            // insert new value number into table using the key
            int value = m_next_value_number++;
            m_value_numbering_map[keyExpr] = value;
            
            // record that new value for T.
            m_value_numbering_map[keyT] = value;
        }
    }
    
    std::cout << "Value Map" << std::endl;
    for (auto it : m_value_numbering_map)
    {
        std::cout << it.second << " [" << it.first.m_left;
        std::cout << "," << IrOpcodeToString(it.first.m_op);
        std::cout << "," << it.first.m_right;
        std::cout << "]" << std::endl;
    }
    
/*    
    for (auto it : m_statements)
    {
        if (!isBinaryOp(it.m_opcode))
            continue;
        
        Key keyExpr(it.m_src0->asString(), it.m_opcode, (it.m_src1 != nullptr) ? it.m_src1->asString() : "");
        auto mip = m_value_numbering_map.find(keyExpr);
        if (mip != m_value_numbering_map.end())
        {
            std::cout << "#" << mip->second << std::endl;
        }
        else
        {
            std::cout << "-" << std::endl;
        }
    }
*/
/*
    for i ← 1 to n
        r ← value number for rhs[i]
        l ← value number for lhs[i]
        if op[i] is a store then
            SYMBOLS[result[i]].val ← r
            if r is constant then
                SYMBOLS[lhs[i]].isConstant ← true
        else // an expression
            if l is constant then replace lhs[i] with constant
            if r is constant then replace rhs[i] with constant
            if l and r are both constant then
                create CONSTANTS(l, op[i], r)
                CONSTANTS(l, op[i], r).bits ← eval(l op[i] r)
                CONSTANTS(l, op[i], r).val ← new value number
                replace (lhs op[i] rhs) with “ constant (l op[i] r)”
            else if (l, op[i], r) ∈ AVAIL then
                replace (lhs op[i] rhs) with “AVAIL(l, op[i], r).result[j]”
            else create AVAIL(l, op[i], r)
                AVAIL(l, op[i], r).val ← new value number
            endif
            SYMBOLS[result[i]].val ← value number of expression
        endif
        for all AVAIL(l, op[ j], r)
            if result[i].val = l or r or result[ j].val, ( j < i)
                remove (l, op[ j], r) from AVAIL
        endfor
    endfor
*/
#if 0
    // Statement of form: T <- L op R
    for (auto it = m_statements.begin(); it != m_statements.end(); ++it)
    {
        if (!isBinaryOp(it->m_opcode))
            continue;
        
        int l = getValueNumber(it->m_src0);
        int r = getValueNumber(it->m_src1);
        int res = getValueNumber(it->m_dst);
        
        if (it->m_opcode == IrOpcode::MOV)
        {
            // dst = arg0 (arg0 is lhs)
            m_symbols[it->m_dst->asString()].m_value_number = l;
            m_symbols[it->m_dst->asString()].m_isConstant = m_symbols[it->m_src0->asString()].m_isConstant;
        }
        else // expression of form: result = lhs op rhs
        {
            //if (m_symbols[it.m_src0->asString()].m_isConstant)
            //{
            //}
            //if (m_symbols[it.m_src1->asString()].m_isConstant)
            //{
            //}
            if (m_symbols[it->m_src0->asString()].m_isConstant && m_symbols[it->m_src1->asString()].m_isConstant)
            {
                // create a new constant C = src0 op src1
            }
            else
            {
                ValueKey key;
                key.m_lhs = l;
                key.m_op = (int)it->m_opcode;
                key.m_rhs = r;
                
                auto ait = m_availables.find(key);
                if (ait != m_availables.end())
                {
                    // replace (lhs op[i] rhs) with “AVAIL(l, op[i], r).result[j]”
                    it->m_src0 = ait->second.m_stmt.m_dst;
                    it->m_opcode = IrOpcode::MOV;
                    it->m_src1 = nullptr;
                }
                else
                {
                    Available avail;
                    avail.m_lhs = key.m_lhs;
                    avail.m_op = key.m_op;
                    avail.m_rhs = key.m_rhs;
                    avail.m_result = res;
                    avail.m_stmt = *it;
                    
                    m_availables[key] = avail;
                }
            }
            
            // Create key from op, L and R.
            std::string exprKey = it->m_src0->asString() + IrOpcodeToString(it->m_opcode) + it->m_src1->asString();                
            auto mip = m_symbols.find(exprKey);
            if (mip == m_symbols.end())
            {
                // insert new value number into table using the key
                Symbol symbol;
                symbol.m_name = exprKey;
                symbol.m_value_number = m_next_value_number++;
                symbol.m_isConstant = false;
                
                m_symbols[exprKey] = symbol;
            }
        }
        /*
        for all AVAIL(l, op[ j], r)
            if result[i].val = l or r or result[ j].val, ( j < i)
                remove (l, op[ j], r) from AVAIL
        endfor
        */
        for (auto iit = m_statements.begin(); iit != it; ++iit)
        {
            
        }
    }
#endif
    
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

int IrBasicBlock::getValueNumber(const std::shared_ptr<IrBase>& arg)
{
    if (arg == nullptr)
        return 0;
    
    auto it = m_symbols.find(arg->asString());
    if (it != m_symbols.end())
    {
        return it->second.m_value_number;
    }
    
    Symbol symbol;
    symbol.m_name = arg->asString();
    symbol.m_value_number = m_next_value_number++;
    symbol.m_isConstant = false;
    
    const IrLiteral* literal = dynamic_cast<const IrLiteral*>(arg.get());
    if (literal != nullptr)
        symbol.m_isConstant = true;
    
    m_symbols[symbol.m_name] = symbol;
    
    return symbol.m_value_number;
}

} // namespace Decaf

