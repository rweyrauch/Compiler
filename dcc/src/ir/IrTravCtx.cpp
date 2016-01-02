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
#include "IrMethodCall.h"
#include "IrStringLiteral.h"
#include "IrDoubleLiteral.h"

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
   
bool IrTraversalContext::addString(IrIdentifier* identifier, const std::string& value)
{
    if (identifier == nullptr) return false;
    
    bool ok = true;
    auto it = m_strings.find(identifier->getIdentifier());
    if (it == m_strings.end())
    {
        SStringSymbol symbol;
        symbol.m_name = identifier->getIdentifier();
        symbol.m_value = value;
        m_strings[identifier->getIdentifier()] = symbol;
    }
    else
    {
        ok = false;
    }
    return ok;
}
   
bool IrTraversalContext::lookup(const std::string& value, SStringSymbol& symbol)
{
    bool found = false;
    
    for (auto it : m_strings)
    {
        if (it.second.m_value == value)
        {
            found = true;
            symbol = it.second;
            break;
        }
    }
    
    return found;
}
   
bool IrTraversalContext::addDouble(IrIdentifier* identifier, double value)
{
    if (identifier == nullptr) return false;
    
    bool ok = true;
    auto it = m_doubles.find(identifier->getIdentifier());
    if (it == m_doubles.end())
    {
        SDoubleSymbol symbol;
        symbol.m_name = identifier->getIdentifier();
        symbol.m_value = value;
        m_doubles[identifier->getIdentifier()] = symbol;
    }
    else
    {
        ok = false;
    }
    return ok;    
}

bool IrTraversalContext::lookup(double value, SDoubleSymbol& symbol)
{
    bool found = false;
    
    for (auto it : m_doubles)
    {
        if (it.second.m_value == value)
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
    if (!m_sourceFilename.empty())
        stream << ".file \"" << m_sourceFilename << "\"" << std::endl;
    
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
        IrTacStmt tac(IrOpcode::STRING);
        tac.m_src0.build(it->second.m_name);
        tac.m_src1.build(it->second.m_value);
        append(tac);
    }
    
    IrTacStmt rtc(IrOpcode::STRING);
    rtc.m_src0.build(".BOUNDSMSG");
    rtc.m_src1.build("*** RUNTIME ERROR ***: Array out of bounds access in file \\\"%s\\\" at line %d.\\n");
    append(rtc);
    
    if (!m_sourceFilename.empty())
    {
        IrTacStmt tac(IrOpcode::STRING);
        tac.m_src0.build(".DCFFILE");
        tac.m_src1.build(m_sourceFilename);
        append(tac);
    }
}
  
void IrTraversalContext::genDoubles()
{
    for (auto it = m_doubles.cbegin(); it != m_doubles.cend(); ++it)
    {
        IrTacStmt tac(IrOpcode::DOUBLE);
        tac.m_src0.build(it->second.m_name);
        tac.m_src1.build(it->second.m_value);
        append(tac);
    }    
}

} // namespace Decaf
