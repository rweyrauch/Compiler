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
#include "IrProgram.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrProgram::~IrProgram()
{
}
    
void IrProgram::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    for (auto it : m_field_decl_list)
    {
        it->propagateTypes(ctx);
    }
    for (auto it : m_method_decl_list)
    {
        it->propagateTypes(ctx);
    }
 
    for (auto it : m_class_decl_list)
    {
        it->propagateTypes(ctx);
    }
    
    for (auto it : m_interface_decl_list)
    {
        it->propagateTypes(ctx);
    }
    
    ctx->popParent();
    ctx->popSymbols();
 }
    
void IrProgram::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    
    std::cout << "Program(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;

    m_symbols->print(depth+1);
    
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
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Class Declarations: " << std::endl;
    for (auto it : m_class_decl_list)
    {
        it->print(depth+2);
    }
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Interface Declarations: " << std::endl;
    for (auto it : m_interface_decl_list)
    {
        it->print(depth+2);
    }
}

bool IrProgram::analyze(IrTraversalContext* ctx) 
{
    bool valid = true;
    
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    // Rule: Valid program must contain a global "main" function with no parameters or
    // a class named "Program" with a "main" function with no parameters.
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
        for (auto it : m_class_decl_list)
        {
            if (it->getIdentifier()->getIdentifier() == "Program")
            {
                const size_t numMethods = it->getNumMethods();
                for (size_t i = 0; i < numMethods; i++)
                {
                    if (it->getMethod(i)->getName() == "main" && it->getMethod(i)->getNumArguments() == 0 && it->getMethod(i)->getReturnType() == IrType::Void)
                    {
                        mainFound = true;
                        break;
                    }
                }
            }
        }
    }
    
    if (!mainFound)
    {
        ctx->error(this, "program must contain a method \'main\'.");
        valid = false;
    }
    
    for (auto it : m_field_decl_list)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    
    for (auto it : m_method_decl_list)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    
    for (auto it : m_class_decl_list)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    
    for (auto it : m_interface_decl_list)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}

bool IrProgram::allocate(IrTraversalContext* ctx) 
{ 
    bool valid = true;
    
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    for (auto it : m_field_decl_list)
    {
        if (!it->allocate(ctx))
            valid = false;
    }
    
    for (auto it : m_method_decl_list)
    {
        if (!it->allocate(ctx))
            valid = false;
    }
    
    for (auto it : m_class_decl_list)
    {
        if (!it->allocate(ctx))
            valid = false;
    }
    
    for (auto it : m_interface_decl_list)
    {
        if (!it->allocate(ctx))
            valid = false;
    }
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid; 
}

bool IrProgram::codegen(IrTraversalContext* ctx) 
{ 
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    for (auto it : m_field_decl_list)
    {
        it->codegen(ctx);
    }
    for (auto it : m_method_decl_list)
    {
        it->codegen(ctx);
    }
    for (auto it : m_class_decl_list)
    {
        it->codegen(ctx);
    }
    for (auto it : m_interface_decl_list)
    {
        it->codegen(ctx);
    }
    
    ctx->popParent();
    ctx->popSymbols();
    
    return true; 
}

void IrProgram::addFieldDecl(IrFieldDeclPtr field)
{
    m_field_decl_list.push_back(field);
    
    m_symbols->addVariable(field.get());    
}

void IrProgram::addFieldDecl(const std::vector<IrFieldDecl*>& fields)
{
    for (auto it : fields)
    {
        addFieldDecl(IrFieldDeclPtr(it));
    }
}

void IrProgram::addMethodDecl(IrMethodDeclPtr method)
{
    m_method_decl_list.push_back(method);
    
    m_symbols->addMethod(method.get());    
}

void IrProgram::addMethodDecl(const std::vector<IrMethodDecl*>& methods)
{
    for (auto it : methods)
    {
        addMethodDecl(IrMethodDeclPtr(it));
    }
}

void IrProgram::addClassDecl(IrClassPtr classDecl)
{
    m_class_decl_list.push_back(IrClassPtr(classDecl));
    
    m_symbols->addClass(classDecl.get());        
}

void IrProgram::addClassDecl(const std::vector<IrClass*>& classes)
{
    for (auto it : classes)
    {
        addClassDecl(IrClassPtr(it));
    }
}

void IrProgram::addInterfaceDecl(IrInterfacePtr interfaceDecl)
{
    m_interface_decl_list.push_back(interfaceDecl);
    
    m_symbols->addInterface(interfaceDecl.get());        
}

void IrProgram::addInterfaceDecl(const std::vector<IrInterface*>& interfaces)
{
    for (auto it : interfaces)
    {
        addInterfaceDecl(IrInterfacePtr(it));
    }
}

} // namespace Decaf
