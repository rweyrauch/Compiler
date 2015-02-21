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
#include "IrMethodDecl.h"
#include "IrVarDecl.h"
#include "IrBlock.h"

namespace Decaf
{

void IrMethodDecl::clean()
{
}

void IrMethodDecl::print(unsigned int depth)
{
    IRPRINT_INDENT(depth);
    std::cout << "Method(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    IRPRINT_INDENT(depth+1);
    std::cout << "Name: " << std::endl;
    m_identifier->print(depth+2);
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Return Type: " << IrTypeToString(m_returnType) << std::endl;
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Arguments: " << std::endl;
    for (auto it : m_argument_list)
    {
        it->print(depth+2);
    }  
    
    if (m_block) 
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "Body: " << std::endl;        
        m_block->print(depth+2);
    }
}
    
bool IrMethodDecl::applySemanticChecks(const std::string& filename)
{
    return true;
}
    
} // namespace Decaf