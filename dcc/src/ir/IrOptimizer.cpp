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
#include <cstring>
#include "IrOptimizer.h"
#include "IrBasicBlock.h"

namespace Decaf
{

void IrOptimizer::generateBasicBlocks(const std::vector<IrTacStmt>& statements)
{
    m_statements = statements;
    
    m_blocks.clear();
    
    auto curBlock = IrBasicBlockPtr(new IrBasicBlock());
    m_blocks.push_back(curBlock);
    
    for (auto it : statements)
    {
        // start a new block when a leader statement is found
        if (isLeader(it))
        {
            // create a new block if needed
            if (!curBlock->isEmpty())
            {
                curBlock = IrBasicBlockPtr(new IrBasicBlock());
                m_blocks.push_back(curBlock);
            }
        }
        
        curBlock->append(it);
        
        // start a new block when a leader statement is found
        if (isLeaderPost(it))
        {
            curBlock = IrBasicBlockPtr(new IrBasicBlock());
            m_blocks.push_back(curBlock);
        }
    }
    
    // Generate adjacency matrix from list of blocks
    m_controlFlowGraphRoots.clear();
    
    const size_t N = m_blocks.size();
    m_blockAdjacencyMat = new unsigned char[N*N];
    memset(m_blockAdjacencyMat, 0, sizeof(unsigned char) * N * N);
    size_t n = 0;
    for (auto it : m_blocks)
    {
        const std::vector<IrTacStmt>& stmts = it->getStatements();
        if (stmts.empty()) continue;
                
        if (stmts.front().m_opcode == IrOpcode::LABEL)
        {
            // find predecessor(s)
            int nb = 0;
            for (auto ib : m_blocks)
            {
                if (ib->isLabelTargetInBlock(stmts.front().m_src0.m_asString))
                {
                    m_blockAdjacencyMat[n * N + nb] = 2;
                }
                nb++;
            }
        }
        else if (stmts.front().m_opcode == IrOpcode::FBEGIN)
        {
            // root of a control flow graph
            m_controlFlowGraphRoots.push_back(n);
        }
        else
        {
            if (n-1 >= 0)
                m_blockAdjacencyMat[n * N + (n-1)] = 2;           
        }
       
        if (stmts.back().m_opcode == IrOpcode::JUMP)
        {
            // find next
            size_t nb = 0;
            for (auto ib : m_blocks)
            {
                if (ib->isLabelTargetInBlock(stmts.back().m_src0.m_asString))
                {
                    m_blockAdjacencyMat[n * N + nb] = 1;
                }
                nb++;
            }            
        }
        else if (stmts.back().m_opcode == IrOpcode::IFZ || stmts.back().m_opcode == IrOpcode::IFNZ)
        {
            if (n+1 < N)
                m_blockAdjacencyMat[n * N + (n+1)] = 1;
            size_t nb = 0;
            for (auto ib : m_blocks)
            {
                if (ib->isLabelTargetInBlock(stmts.back().m_src1.m_asString))
                {
                    m_blockAdjacencyMat[n * N + nb] = 1;
                }
                nb++;
            }
        }
        else if (stmts.back().m_opcode != IrOpcode::RETURN)
        {
            if (n+1 < N)
                m_blockAdjacencyMat[n * N + (n+1)] = 1;                
        }
        n++;
    }
}

void IrOptimizer::basicBlocksOptimizations(IrBasicBlockOpts which)
{    
    m_statements.clear();
    for (auto it : m_blocks)
    {
        it->optimize(which);
        
        const std::vector<IrTacStmt>& stmts = it->getStatements();
        if (stmts.empty()) continue;
        
        for (auto sit : stmts)
        {
            m_statements.push_back(sit);
        }
    }        
}

void IrOptimizer::globalCommonSubexpressionElimination()
{
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
    size_t n = 0;
    std::cout << "---------------------------" << std::endl;
    for (auto it : m_blocks)
    {
        std::cout << "Block[" << n << "]:  NumStatements: " << it->getStatements().size() << std::endl;
        it->print(stream);
        std::cout << "---------------------------" << std::endl;
        n++;
    }
    
    std::cout << "Control Flow Graph Roots: ";
    for (auto it : m_controlFlowGraphRoots)
    {
        std::cout << (unsigned int)it << " ";
    }
    std::cout << std::endl;
    
    const size_t N = m_blocks.size();
    
    for (n = 0; n < N; n++)
    {
        std::cout << " ";
        if (n >= 10) std::cout << n/10;
        else std::cout << " ";
    }
    std::cout << std::endl;
    for (n = 0; n < N; n++)
    {
        std::cout << " " << n%10;
    }
    std::cout << std::endl;
    for (n = 0; n < N; n++)
    {
        std::cout << "--";
    }
    std::cout << std::endl;
    
    for (n = 0; n < N; n++)
    {
        for (size_t nn = 0; nn < N; nn++)
        {
            std::cout << " " << (unsigned int)m_blockAdjacencyMat[n * N + nn];
        }
        std::cout << " | " << n << std::endl;
    }
}

} // namespace Decaf
