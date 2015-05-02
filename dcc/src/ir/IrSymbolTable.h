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
#pragma once
#include <map>
#include "IrCommon.h"
#include "IrBase.h"
#include "IrFieldDecl.h"
#include "IrVarDecl.h"

namespace Decaf
{
class IrMethodCall;
class IrMethodDecl;
class IrLocation;
class IrIdentifier;
class IrClass;
class IrInterface;

class IrSymbolTable : public IrBase
{
public:
    IrSymbolTable() :
        IrBase(0, 0, __FILE__),
        m_startAddr(0),
        m_variableStackSize(0),
        m_variables(),
        m_methods()
    {}
    
    ~IrSymbolTable() 
    {}
    
    bool addVariable(IrFieldDecl* variable);
    bool addVariable(IrVariableDecl* variables);
    bool addVariable(IrLocation* variable);
    bool addVariable(IrIdentifier* variable, IrType type);
    
    bool addMethod(IrMethodDecl* method);
    bool addClass(IrClass* classDecl);
    bool addInterface(IrInterface* interfaceDecl);
    
    bool exists(IrLocation* variable) const;
    bool exists(IrMethodCall* method) const;
    
    bool getSymbol(IrLocation* variable, SVariableSymbol& symbol) const;
    bool getSymbol(IrIdentifier* variable, SVariableSymbol& symbol) const;
    bool getSymbol(IrMethodCall* method, SMethodSymbol& symbol) const;
    
    size_t getAllocationSize() const;
    void setStartAddress(ptrdiff_t addr) { m_startAddr = addr; }
    
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx) { return true; }
    virtual bool allocate(IrTraversalContext* ctx) { return true; }
    
protected:
        
    ptrdiff_t m_startAddr;    
    size_t m_variableStackSize;    
    std::map<std::string, SVariableSymbol> m_variables;
    
    std::map<std::string, SMethodSymbol> m_methods;
};
    
} // namespace Decaf
