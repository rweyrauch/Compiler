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
#include <sstream>
#include "IrCommon.h"
#include "IrIdentifier.h"
#include "IrTravCtx.h"

namespace Decaf
{

int IrIdentifier::s_tempCounter = 0;

IrIdentifier* IrIdentifier::CreateTemporary()
{
    std::stringstream tempName;
    tempName << ".LC" << s_tempCounter++;
    IrIdentifier* tempId = new IrIdentifier(0, 0, "temp", tempName.str());
    return tempId;
}

IrIdentifier* IrIdentifier::CreateLabel()
{
    std::stringstream tempName;
    tempName << ".L" << s_tempCounter++;
    IrIdentifier* tempId = new IrIdentifier(0, 0, "label", tempName.str(), true);
    return tempId;
}

void IrIdentifier::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Identifier(" << getLineNumber() << "," << getColumnNumber() << ") = \"" << m_identifier << "\"" << std::endl;
}

bool IrIdentifier::codegen(IrTraversalContext* ctx)
{
    bool ok = false;
    SVariableSymbol symbol;
    
    if (ctx->lookup(this, symbol))
    {
        m_isGlobal = (symbol.m_where == IrMemLocation::Global);
        m_addr = symbol.m_addr;
        
        ok = true;
    }
    
    return ok;
}

} // namespace Decaf
