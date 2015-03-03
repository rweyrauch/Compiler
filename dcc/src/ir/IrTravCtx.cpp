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
#include "IrTravCtx.h"
#include "IrSymbolTable.h"
#include "IrIdentifier.h"
#include "IrMethodCall.h"

namespace Decaf
{

const IrBase* IrTraversalContext::getParent(size_t generation) const
{
    if (generation < m_parents.size()) 
    {
        return (m_parents.at(m_parents.size() - 1 - generation));
    }
    return nullptr;
}

bool IrTraversalContext::addTempVariable(IrIdentifier* variable, IrType type)
{
    if (!m_symbols.empty())
    {
        return m_symbols.front()->addVariable(variable, type);
    }
    return false;
}

bool IrTraversalContext::lookup(IrIdentifier* variable, SVariableSymbol& symbol) const
{
    bool found = false;
    // TODO: make this a vector with reverse iterator
    for (auto it : m_symbols)
    {
        if (it->getSymbol(variable, symbol))
        {
            found = true;
            break;
        }
    }
    return found;
}

bool IrTraversalContext::lookup(IrLocation* location, SVariableSymbol& symbol) const
{
    return lookup(location->getIdentifier(), symbol);
}

bool IrTraversalContext::lookup(IrMethodCall* method, SMethodSymbol& symbol) const
{
    bool found = false;
    // TODO: make this a vector with reverse iterator
    for (auto it : m_symbols)
    {
        if (it->getSymbol(method, symbol))
        {
            found = true;
            break;
        }
    }
    return found;
}
   
const std::string& IrTraversalContext::sourceAt(int line_num) const
{
    
    if (m_source && line_num > 0 && line_num <= (int)m_source->size())
    {
        return m_source->at((size_t)line_num-1);
    }
    return m_blank;
}
   
void IrTraversalContext::error(const IrBase* node, const std::string& message) const
{
    if (node != nullptr)
    {
        std::cerr << node->getFilename() << ":" << node->getLineNumber() << ":" << node->getColumnNumber() << ": error: " << message << std::endl;
        highlightError(node->getLineNumber(), node->getColumnNumber());
    }
}
   
void IrTraversalContext::highlightError(int line, int column, int length) const
{
    const std::string& bad_line = sourceAt(line);
    if (bad_line.size() > 0)
    {
        std::cerr << bad_line << std::endl;
        for (int i = 0; i < column; i++) std::cerr << " ";
        for (int i = 0; i < length; i++) std::cerr << "^";
        std::cerr << std::endl;
    }    
}
  
} // namespace Decaf