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
#include <iostream>
#include <vector>
#include <memory>
#include <list>
#include "IrCommon.h"
#include "IrBasicBlock.h"
#include "IrTAC.h"

namespace Decaf
{
class IrTraversalContext;

class IrOptimizer
{
public:
    IrOptimizer() :
        m_blocks(),
        m_statements(),
        m_blockAdjacencyMat(nullptr),
        m_controlFlowGraphRoots(),
        m_controlFlowGraphExits()
    {}
    
    virtual ~IrOptimizer() 
    {
        delete[] m_blockAdjacencyMat;
    }
   
    void generateBasicBlocks(const std::vector<IrTacStmt>& statements);
    void basicBlocksOptimizations(IrBasicBlockOpts which);
    void globalCommonSubexpressionElimination();
    void generateStatements();
    
    const std::vector<IrTacStmt>& getOptimizedStatements() const { return m_statements; }
    
    void print(std::ostream& stream = std::cout);
    
protected:

    bool isLeader(const IrTacStmt& stmt);
    bool isLeaderPost(const IrTacStmt& stmt);

    void printControlFlowGraphs(std::ostream& stream);
    
protected:
    
    std::vector<IrBasicBlockPtr> m_blocks;
    std::vector<IrTacStmt> m_statements;
   
    // NxN block adjacency matrix
    unsigned char* m_blockAdjacencyMat;
    std::vector<unsigned int> m_controlFlowGraphRoots;
    std::vector<unsigned int> m_controlFlowGraphExits;
    
private:
    IrOptimizer(const IrOptimizer& rhs) = delete;
};

} // namespace Decaf
