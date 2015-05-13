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
#include "IrStringLiteral.h"

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
        if (m_symbols.front()->addVariable(variable, type))
        {
            return variable->allocate(this);
        }
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
    return lookup(location->getIdentifier().get(), symbol);
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
   
bool IrTraversalContext::addString(IrIdentifier* identifier, IrStringLiteral* value)
{
    bool ok = true;
    auto it = m_strings.find(identifier->getIdentifier());
    if (it == m_strings.end())
    {
        SStringSymbol symbol;
        symbol.m_name = std::shared_ptr<IrIdentifier>(identifier);
        symbol.m_value = std::shared_ptr<IrStringLiteral>(value);
        m_strings[identifier->getIdentifier()] = symbol;
    }
    else
    {
        // Error - duplicate
        std::cerr << identifier->getFilename() << ":" << identifier->getLineNumber() << ":" << identifier->getColumnNumber() << ": error: string \'" << identifier->getIdentifier() << "\' already declared in scope." << std::endl;                
        ok = false;
    }
    return ok;    
}
   
bool IrTraversalContext::lookup(const std::string& value, SStringSymbol& symbol)
{
    bool found = false;
    
    for (auto it : m_strings)
    {
        if (it.second.m_value->getValue() == value)
        {
            found = true;
            symbol = it.second;
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
  
void IrTraversalContext::codegen(std::ostream& stream)
{
    if (!m_ia64)
        IrGenIA32();

    stream << ".text" << std::endl;
    for (auto it : m_statements)
    {
        IrTacGenCode(it, stream);
    }
}
  
void IrTraversalContext::genStrings()
{
    for (auto it = m_strings.cbegin(); it != m_strings.cend(); ++it)
    {
        IrTacStmt tac;
        tac.m_opcode = IrOpcode::STRING;
        tac.m_src0.build(it->second.m_name.get());
        tac.m_src1.build(it->second.m_value.get());
        append(tac);
    }
    
    std::shared_ptr<IrIdentifier> label = std::shared_ptr<IrIdentifier>(new IrIdentifier(__LINE__, 0, __FILE__, ".BOUNDSMSG", true));
    std::shared_ptr<IrStringLiteral> value = std::shared_ptr<IrStringLiteral>(new IrStringLiteral(__LINE__, 0, __FILE__, "\"Bounds check failed.\""));
    IrTacStmt rtc;
    rtc.m_opcode = IrOpcode::STRING;
    rtc.m_src0.build(label.get());
    rtc.m_src1.build(value.get());
    append(rtc);
}
  
} // namespace Decaf
