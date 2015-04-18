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
    int nextValueNumber = 42;
    
    m_value_numbering_map.clear();
        
    // Statement of form: T <- L op R
    for (auto it : m_statements)
    {
        if (!isBinaryOp(it.m_opcode))
            continue;
        
        // Get/create the value numbers of T, L and R
        Key keyL(it.m_arg0, IrOpcode::NOOP, nullptr);
        auto lip = m_value_numbering_map.find(keyL);
        if (lip == m_value_numbering_map.end())
        {
            m_value_numbering_map[keyL] = nextValueNumber++;
        }
        
        if (it.m_arg1 != nullptr)
        {
            Key keyR(it.m_arg1, IrOpcode::NOOP, nullptr);
            auto rip = m_value_numbering_map.find(keyR);
            if (rip == m_value_numbering_map.end())
            {
                m_value_numbering_map[keyR] = nextValueNumber++;
            }
        }
        
        Key keyT(it.m_arg2, IrOpcode::NOOP, nullptr);
        auto tip = m_value_numbering_map.find(keyT);
        if (tip == m_value_numbering_map.end())
        {
            m_value_numbering_map[keyT] = nextValueNumber++;
        }
        
        // Create key from op, L and R.
        Key keyExpr(it.m_arg0, it.m_opcode, it.m_arg1);
                
        auto mip = m_value_numbering_map.find(keyExpr);
        if (mip != m_value_numbering_map.end())
        {
            m_value_numbering_map[keyT] = mip->second;
        }
        else
        {
            // insert new value number into table using the key
            int value = nextValueNumber++;
            m_value_numbering_map[keyExpr] = value;
            
            // record that new value for T.
            m_value_numbering_map[keyT] = value;
        }
    }
    
    std::cout << "Value Map" << std::endl;
    for (auto it : m_value_numbering_map)
    {
        const IrIdentifier* identL = dynamic_cast<const IrIdentifier*>(it.first.m_left.get());
        const IrIdentifier* identR = dynamic_cast<const IrIdentifier*>(it.first.m_right.get());
        const IrLiteral* litL = dynamic_cast<const IrLiteral*>(it.first.m_left.get());
        const IrLiteral* litR = dynamic_cast<const IrLiteral*>(it.first.m_right.get());
        std::cout << it.second << " [";
        if (identL)
            std::cout << identL->getIdentifier();
        else if (litL)
            std::cout << "$" << litL->getValueAsString();
        std::cout << "," << IrOpcodeToString(it.first.m_op);
        if (identR)
            std::cout << "," << identR->getIdentifier();
        else if (litR) 
            std::cout << ",$" << litR->getValueAsString();
        std::cout << "]" << std::endl;
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

