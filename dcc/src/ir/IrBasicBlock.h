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

class IrBasicBlock
{
public:
    IrBasicBlock() :
        m_statements(),
        m_value_numbering_map()
    {}
    
    virtual ~IrBasicBlock() 
    {}
    
    void append(const IrTacStmt& tac)
    {
        m_statements.push_back(tac);
    }
    
    const std::vector<IrTacStmt>& getStatements() const { return m_statements; }
    
    bool commonSubexpressionElimination();
    
    bool codegen(IrTraversalContext* ctx);
    
    void print(std::ostream& stream);
    
protected:
    
    std::vector<IrTacStmt> m_statements;
    
    struct Key
    {
        Key(const std::string& left, IrOpcode opcode, const std::string& right) :
            m_left(left),
            m_op(opcode),
            m_right(right) {}
        Key() :
            m_left(""),
            m_op(IrOpcode::NOOP),
            m_right("") {}
            
        std::string m_left;
        IrOpcode m_op;
        std::string m_right;
        
        bool operator<(const Key& lhs) const
        {
            if (m_left < lhs.m_left)
                return true;
            else if ((int)m_op < (int)lhs.m_op)
                return true;
            else if (m_right < lhs.m_right)
                return true;
            return false;
        }
        bool operator==(const Key& lhs) const
        {
            if ((m_left == lhs.m_left) &&
                (m_op == lhs.m_op) &&
                (m_right == lhs.m_right))
                return true;
            return false;
        }
    };
    
    struct KeyHasher
    {
        size_t operator()(const Key& key) const
        {
            return ((std::hash<std::string>()(key.m_left) ^
                    (std::hash<int>()((int)key.m_op) << 1)) >> 1) ^
                    (std::hash<std::string>()(key.m_right) << 1);
        }
    };
    
    std::unordered_map<Key, int, KeyHasher> m_value_numbering_map;
    
    struct Symbol
    {
        std::string m_name;
        int m_value_number;
        bool m_isConstants;
    };
    
    std::map<std::string, Symbol> m_symbols;
    
    struct Constant
    {
        int m_value_number;
        IrType m_type;
        int m_value_int;
        double m_value_double;
        bool m_value_bool;
    };
    
    struct ValueKey
    {
        int m_lhs;
        int m_op;
        int m_rhs;
    };
    
    struct Available
    {
        int m_lhs;
        int m_op;
        int m_rhs;
        int m_result;
        IrTacStmt m_stmt;
    };
    
    std::map<ValueKey, Constant> m_constants;  
    std::map<ValueKey, Available> m_availables;
    
private:
    IrBasicBlock(const IrBasicBlock& rhs) = delete;
};

} // namespace Decaf
