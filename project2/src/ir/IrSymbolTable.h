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
#include "IrFieldDecl.h"
#include "IrVarDecl.h"
#include "IrMethodDecl.h"

namespace Decaf
{
class IrMethodCall;
class IrLocation;

class IrSymbolTable
{
public:
    IrSymbolTable() :
        m_variables()
    {}
    
    ~IrSymbolTable() 
    {}
    
    bool addVariable(IrFieldDecl* variable);
    bool addVariable(IrVariableDecl* variables);
    bool addMethod(IrMethodDecl* method);
    
    bool exists(IrLocation* variable) const;
    bool exists(IrMethodCall* method) const;
    
    void print(int depth);
    
protected:
    
    struct SVariableSymbol
    {
        std::string m_name;
        IrType m_type;
        size_t m_count; // >1 for arrays
    };
    
    std::map<std::string, SVariableSymbol> m_variables;
    
    struct SMethodSymbol
    {
        std::string m_name;
        IrType m_type;
        std::vector<SVariableSymbol> m_arguments;
    };
    
    std::map<std::string, SMethodSymbol> m_methods;
    
};
    
} // namespace Decaf