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
#include "IrSymbolTable.h"
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

void IrProgram::addFieldDecl(IrFieldDecl* field)
{
    m_field_decl_list.push_back(std::shared_ptr<IrFieldDecl>(field));
    
    m_symbols->addVariable(field);    
}

void IrProgram::addFieldDecl(const std::vector<IrFieldDecl*>& fields)
{
    for (auto it : fields)
    {
        addFieldDecl(it);
    }
}

void IrProgram::addMethodDecl(IrMethodDecl* method)
{
    m_method_decl_list.push_back(std::shared_ptr<IrMethodDecl>(method));
    
    m_symbols->addMethod(method);    
}

void IrProgram::addMethodDecl(const std::vector<IrMethodDecl*>& methods)
{
    for (auto it : methods)
    {
        addMethodDecl(it);
    }
}

void IrProgram::addClassDecl(IrClass* classDecl)
{
    m_class_decl_list.push_back(std::shared_ptr<IrClass>(classDecl));
    
    m_symbols->addClass(classDecl);        
}

void IrProgram::addClassDecl(const std::vector<IrClass*>& classes)
{
    for (auto it : classes)
    {
        addClassDecl(it);
    }
}

void IrProgram::addInterfaceDecl(IrInterface* interfaceDecl)
{
    m_interface_decl_list.push_back(std::shared_ptr<IrInterface>(interfaceDecl));
    
    m_symbols->addInterface(interfaceDecl);        
}

void IrProgram::addInterfaceDecl(const std::vector<IrInterface*>& interfaces)
{
    for (auto it : interfaces)
    {
        addInterfaceDecl(it);
    }
}

} // namespace Decaf
