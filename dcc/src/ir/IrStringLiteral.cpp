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
#include "IrStringLiteral.h"
#include "IrTravCtx.h"

namespace Decaf
{
   
IrStringLiteral::IrStringLiteral(int lineNumber, int columnNumber, const std::string& filename, const std::string& value) :
    IrLiteral(lineNumber, columnNumber, filename, IrType::String, value),
    m_label(nullptr)
{
    if (m_valueAsString.front() == '"')
    {
        m_valueAsString.erase(0, 1);
    }
    if (m_valueAsString.back() == '"')
    {
        m_valueAsString.pop_back();
    }
}
    
void IrStringLiteral::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "String(" << getLineNumber() << "," << getColumnNumber() << ") = \"" << getValue() << "\"" << std::endl;
}
    
bool IrStringLiteral::codegen(IrTraversalContext* ctx)
{
    bool ok = true;
    SStringSymbol symbol;
    if (!ctx->lookup(getValue(), symbol))
    {
        m_label = IrIdentifier::CreateLabel();
        ok = ctx->addString(m_label.get(), getValue());
        if (!ok)
        {
            // Error - duplicate
            std::cerr << getFilename() << ":" << getLineNumber() << ":" << getColumnNumber() << ": error: string \'" << m_label->getIdentifier() << "\' already declared in scope." << std::endl;                            
        }
    }
    else
    {
        m_label = IrIdentifierPtr(new IrIdentifier(getLineNumber(), getColumnNumber(), getFilename(), symbol.m_name, true));
    }
    
    return ok;
}

}
