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
#include "IrOptimizer.h"
#include "IrBasicBlock.h"

namespace Decaf
{

void IrOptimizer::generateBasicBlocks(const std::vector<IrTacStmt>& statements)
{
    m_statements = statements;
    
    m_blocks.clear();
    
    auto curBlock = std::shared_ptr<IrBasicBlock>(new IrBasicBlock());;
    m_blocks.push_back(curBlock);
    
    for (auto it : statements)
    {
        // start a new block when a leader statement is found
        if (isLeader(it))
        {
            curBlock = std::shared_ptr<IrBasicBlock>(new IrBasicBlock());
            m_blocks.push_back(curBlock);
        }
        
        curBlock->append(it);
        
        // start a new block when a leader statement is found
        if (isLeaderPost(it))
        {
            curBlock = std::shared_ptr<IrBasicBlock>(new IrBasicBlock());
            m_blocks.push_back(curBlock);
        }
    }
    
    // Generate control flow graph from list of blocks
    for (auto it : m_blocks)
    {
        const std::vector<IrTacStmt>& stmts = it->getStatements();
        if (stmts.empty()) continue;
        
        // Create a root control graph for each function/procedure in the block list.
        if (stmts.begin()->m_opcode == IrOpcode::FBEGIN)
        {
            ControlFlowNode node;
            node.m_block = it.get();
            node.m_next_blocks.clear();
            
            m_control_graphs.push_back(node);
        }
        else if (stmts.begin()->m_opcode == IrOpcode::LABEL)
        {
            // TODO: find the parent of this block looking for the LABEL in IFZ, IFNZ and JUMP statements
            if (!m_control_graphs.empty())
                m_control_graphs.back().m_next_blocks.push_back(it.get());
        }
        else
        {
            if (!m_control_graphs.empty())
                m_control_graphs.back().m_next_blocks.push_back(it.get());
        }
    }
}

void IrOptimizer::globalCommonSubexpressionElimination()
{
    for (auto it : m_control_graphs)
    {
        it.m_block->commonSubexpressionElimination();
        for (auto bit : it.m_next_blocks)
        {
            bit->commonSubexpressionElimination();
        }
    }
    
    m_statements.clear();
    for (auto it : m_blocks)
    {
        const std::vector<IrTacStmt>& stmts = it->getStatements();
        if (stmts.empty()) continue;
        
        for (auto sit : stmts)
        {
            m_statements.push_back(sit);
        }
    }    
}

bool IrOptimizer::isLeaderPost(const IrTacStmt& stmt)
{
    switch (stmt.m_opcode)
    {
        case IrOpcode::CALL:
        case IrOpcode::RETURN:
        case IrOpcode::JUMP:
        case IrOpcode::IFZ:
        case IrOpcode::IFNZ:
            return true;
        default:
            break;
    }
    return false;
}

bool IrOptimizer::isLeader(const IrTacStmt& stmt)
{
    switch (stmt.m_opcode)
    {
        case IrOpcode::LABEL:
            return true;
        default:
            break;
    }
    return false;
}

void IrOptimizer::print(std::ostream& stream)
{
    //for (auto it : m_blocks)
    //{
    //    it->print(stream);
    //}
    
    stream << "Control graph nodes." << std::endl;
    for (auto it : m_control_graphs)
    {
        stream << "Procedure Start" << std::endl;
        it.m_block->print(stream);
        for (auto nit : it.m_next_blocks)
        {
            nit->print(stream);
        }
    }
}

} // namespace Decaf
