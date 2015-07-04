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
#include <map>
#include <unordered_map>
#include "IrTAC.h"
#include "IrCommon.h"

namespace Decaf
{
class IrTraversalContext;
class IrBasicBlock;
typedef std::shared_ptr<IrBasicBlock> IrBasicBlockPtr;

class IrBasicBlock
{
public:
    IrBasicBlock() :
        m_statements(),
        m_next_value_number(42),
        m_verbose(false),
        m_gen(),
        m_kill()
    {}
    
    virtual ~IrBasicBlock() 
    {}
    
    void append(const IrTacStmt& tac)
    {
        m_statements.push_back(tac);
    }
    bool isEmpty() const { return m_statements.empty(); }
    
    const std::vector<IrTacStmt>& getStatements() const { return m_statements; }
    
    bool isLabelUsedInBlock(const std::string& label) const;
    bool isLabelDefinedInBlock(const std::string& label) const;
    
    void optimize(IrBasicBlockOpts which);
    void generateDefinitions();  
    
    void print(std::ostream& stream);
    
protected:
    
    std::vector<IrTacStmt> m_statements;
            
    int m_next_value_number;
    
    bool m_verbose;

    std::vector<bool> m_gen, m_kill;
    
protected:
    
    int getValueNumber(const std::string& ident, std::unordered_map<std::string, int>& variable_value_map);
    
    void constantFolding();
    void algebraicSimplification();
    void commonSubexpressionElimination();
    void copyPropagation();
    void deadCodeElimination();
    
private:
    IrBasicBlock(const IrBasicBlock& rhs) = delete;
};

} // namespace Decaf
