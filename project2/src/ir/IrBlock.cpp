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
#include "IrBlock.h"
#include "IrVarDecl.h"
#include "IrStatement.h"

namespace Decaf
{

void IrBlock::clean()
{
    for (auto it : m_variables)
    {
        it->clean();
    }
    for (auto it : m_statements)
    {
        it->clean();
    }
}

void IrBlock::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Block(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    for (auto it : m_variables)
    {
        it->print(depth+1);
    }
    for (auto it : m_statements)
    {
        it->print(depth+1);
    }
}
    
bool IrBlock::applySemanticChecks(const std::string& filename)
{
    bool valid = true;
    for (auto it : m_variables)
    {
        valid = it->applySemanticChecks(filename);
        if (!valid) return valid;
    }
    for (auto it : m_statements)
    {
        valid = it->applySemanticChecks(filename);
        if (!valid) return valid;
    }   
    return valid;
}
    
} // namespace Decaf