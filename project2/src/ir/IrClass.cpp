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
#include "IrClass.h"
#include "IrSymbolTable.h"

namespace Decaf
{

IrClass::~IrClass()
{
    delete m_identifier;
    for (auto it : m_field_decl_list)
        delete it;
    for (auto it : m_method_decl_list)
        delete it;
    delete m_symbols;
}
    
void IrClass::clean()
{
    m_identifier->clean();
    for (auto it : m_field_decl_list)
    {
        it->clean();
    }
    for (auto it : m_method_decl_list)
    {
        it->clean();
    }
 }
    
void IrClass::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    
    std::cout << "Class(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    m_identifier->print(depth+1);
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Field Declarations: " << std::endl;
    for (auto it : m_field_decl_list)
    {
        it->print(depth+2);
    }
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Method Declarations: " << std::endl;
    for (auto it : m_method_decl_list)
    {
        it->print(depth+2);
    }
    
    m_symbols->print(depth);
}

bool IrClass::applySemanticChecks(const std::string& filename) 
{ 
    // Rule: identifier == "Program"
    if (m_identifier->getIdentifier() != "Program")
    {
        std::cerr << "File: " << filename << " Syntax error: Class must be named \'Program\'." << std::endl;
        return false;
    }
    
    // Rule: Valid program must contain a main function with no parameters.
    bool mainFound = false;
    for (auto it : m_method_decl_list)
    {
        if (it->getName() == "main" && it->getNumArguments() == 0 && it->getReturnType() == IrType::Void)
        {
            mainFound = true;
            break;
        }
    }
    if (!mainFound)
    {
        std::cerr << "File: " << filename << " Syntax error: Class must contain a method \'main\'." << std::endl;
        return false;        
    }
    
    for (auto it : m_field_decl_list)
    {
        if (!it->applySemanticChecks(filename))
            return false;
    }
    
    for (auto it : m_method_decl_list)
    {
        if (!it->applySemanticChecks(filename))
            return false;
    }
     
    for (auto it : m_field_decl_list)
    {
        if (!m_symbols->addVariable(it))
            return false;            
    }
    
    return true;
}

void IrClass::addFieldDecl(IrFieldDecl* field)
{
    m_field_decl_list.push_back(field);
}
void IrClass::addFieldDecl(const std::vector<IrFieldDecl*>& fields)
{
    m_field_decl_list.insert(m_field_decl_list.end(), fields.begin(), fields.end());
}

void IrClass::addMethodDecl(IrMethodDecl* method)
{
    m_method_decl_list.push_back(method);
}
void IrClass::addMethodDecl(const std::vector<IrMethodDecl*>& methods)
{
    m_method_decl_list.insert(m_method_decl_list.end(), methods.begin(), methods.end());
}

} // namespace Decaf
