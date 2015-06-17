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
#include "IrTAC.h"

namespace Decaf
{
class IrTraversalContext;
class IrBasicBlock;

class IrOptimizer
{
public:
    IrOptimizer()
    {}
    
    virtual ~IrOptimizer() 
    {}
   
    void generateBasicBlocks(const std::vector<IrTacStmt>& statements);
    void basicBlocksOptimizations(IrBasicBlockOpts which);
    void globalCommonSubexpressionElimination();

    const std::vector<IrTacStmt>& getOptimizedStatements() const { return m_statements; }
    
    void print(std::ostream& stream = std::cout);
    
protected:

    bool isLeader(const IrTacStmt& stmt);
    bool isLeaderPost(const IrTacStmt& stmt);

protected:
    
    std::vector<std::shared_ptr<IrBasicBlock>> m_blocks;
    std::vector<IrTacStmt> m_statements;
   
    // A node in the control flow graph, a block and a list of all of its following blocks.
    struct ControlFlowNode
    {
        std::shared_ptr<IrBasicBlock> m_block;
        std::vector<ControlFlowNode*> m_next_nodes, m_previous_nodes;
    };
    
    std::list<ControlFlowNode*> m_control_graphs; // each function gets its own graph
 
    void insertBlock(std::shared_ptr<IrBasicBlock> block, ControlFlowNode* graph);
     
private:
    IrOptimizer(const IrOptimizer& rhs) = delete;
};

} // namespace Decaf
