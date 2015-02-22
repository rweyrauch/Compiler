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
#include "IrCommon.h"
#include "IrSymbolTable.h"
#include "IrIntLiteral.h"

namespace Decaf
{
  
bool IrSymbolTable::addVariable(IrFieldDecl* variable)
{
    auto it = m_variables.find(variable->getName());
    if (it == m_variables.end())
    {
        SVariableSymbol symbol;
        symbol.m_name = variable->getName();
        symbol.m_type = variable->getType();
        symbol.m_count = 1;
        
        if (variable->getLocation()->getIndex() != nullptr)
        {
            IrIntegerLiteral* size = dynamic_cast<IrIntegerLiteral*>(variable->getLocation()->getIndex());
            if (size)
            {
                symbol.m_count = size->getValue();
            }
            else
            {
                // Error - array size must be a integer literal
                return false;
            }
        }
        m_variables[symbol.m_name] = symbol;
        
        return true;
    }
    
    // error - duplicate symbol
    return false;
}

bool IrSymbolTable::addVariable(IrVariableDecl* variables)
{
    bool ok = true;
    for (size_t i = 0; i < variables->getNumVariables(); i++)
    {
        auto variable = variables->getVariable(i);
        if (variable)
        {
            auto it = m_variables.find(variable->getIdentifier());
            if (it == m_variables.end())
            {
                SVariableSymbol symbol;
                symbol.m_name = variable->getIdentifier();
                symbol.m_type = variables->getType();
                symbol.m_count = 1;
                m_variables[symbol.m_name] = symbol;
            }
            else
            {
                // Error - duplicate
                ok = false;
            }
        }
    }
    return ok;
}
 
void IrSymbolTable::print(int depth)
{
    const size_t table_size = m_variables.size();
    
    if (table_size == 0) return;
    
    std::cout << "Symbol Table Size = " << table_size << std::endl;
    std::cout << "Name\t\tType\t\tCount" << std::endl;
    for (auto it = m_variables.cbegin(); it != m_variables.cend(); ++it)
    {
        std::cout << it->first << "\t\t" << IrTypeToString(it->second.m_type) << "\t\t" << it->second.m_count << std::endl;
    }
}
 
} // namespace Decaf