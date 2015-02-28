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
#include "IrSymbolTable.h"
#include "IrIntLiteral.h"
#include "IrMethodCall.h"
#include "IrMethodDecl.h"

namespace Decaf
{
  
bool IrSymbolTable::addVariable(IrFieldDecl* variable)
{
    auto it = m_variables.find(variable->getName());
    if (it == m_variables.end())
    {
        SVariableSymbol symbol;
        symbol.m_name = variable->getName();
        symbol.m_type = variable->getType();
        symbol.m_count = 1;
        
        if (variable->getLocation()->getIndex() != nullptr)
        {
            IrIntegerLiteral* size = dynamic_cast<IrIntegerLiteral*>(variable->getLocation()->getIndex());
            if (size)
            {
                if (size->getValue() > 0)
                {
                    symbol.m_count = size->getValue();
                }
                else
                {
                    // Error - array size must be greater than zero
                    std::cerr << size->getFilename() << ":" << size->getLineNumber() << ":" << size->getColumnNumber() << ": error: field \'" << variable->getName() << "\' array size must be integer greater than zero." << std::endl;
                    return false;
                }
            }
            else
            {
                // Error - array size must be a integer literal
                std::cerr << size->getFilename() << ":" << size->getLineNumber() << ":" << size->getColumnNumber() << ": error: field \'" << variable->getName() << "\' array size must be integer greater than zero." << std::endl;
                return false;
            }
        }
        m_variables[symbol.m_name] = symbol;
        
        return true;
    }
    
    // error - duplicate symbol
    std::cerr << variable->getFilename() << ":" << variable->getLineNumber() << ":" << variable->getColumnNumber() << ": error: field \'" << variable->getName() << "\' of type " << IrTypeToString(variable->getType()) << " already declared in scope." << std::endl;
    
    return false;
}

bool IrSymbolTable::addVariable(IrVariableDecl* variables)
{
    bool ok = true;
    for (size_t i = 0; i < variables->getNumVariables(); i++)
    {
        auto variable = variables->getVariable(i);
        if (variable)
        {
            auto it = m_variables.find(variable->getIdentifier());
            if (it == m_variables.end())
            {
                SVariableSymbol symbol;
                symbol.m_name = variable->getIdentifier();
                symbol.m_type = variables->getType();
                symbol.m_count = 1;
                m_variables[symbol.m_name] = symbol;
            }
            else
            {
                // Error - duplicate
                std::cerr << variable->getFilename() << ":" << variable->getLineNumber() << ":" << variable->getColumnNumber() << ": error: variable \'" << variable->getIdentifier() << "\' of type " << IrTypeToString(variables->getType()) << " already declared in scope." << std::endl;                
                ok = false;
            }
        }
    }
    return ok;
}
 
bool IrSymbolTable::addVariable(IrLocation* variable)
{
   auto it = m_variables.find(variable->getIdentifier()->getIdentifier());
    if (it == m_variables.end())
    {
        SVariableSymbol symbol;
        symbol.m_name = variable->getIdentifier()->getIdentifier();
        symbol.m_type = variable->getType();
        symbol.m_count = 1;
        
        if (variable->getIndex() != nullptr)
        {
            IrIntegerLiteral* size = dynamic_cast<IrIntegerLiteral*>(variable->getIndex());
            if (size)
            {
                if (size->getValue() > 0)
                {
                    symbol.m_count = size->getValue();
                }
                else
                {
                    // Error - array size must be greater than zero
                    std::cerr << size->getFilename() << ":" << size->getLineNumber() << ":" << size->getColumnNumber() << ": error: field \'" << variable->getIdentifier()->getIdentifier() << "\' array size must be integer greater than zero." << std::endl;
                    return false;
                }
            }
            else
            {
                // Error - array size must be a integer literal
                std::cerr << size->getFilename() << ":" << size->getLineNumber() << ":" << size->getColumnNumber() << ": error: field \'" << variable->getIdentifier()->getIdentifier() << "\' array size must be integer greater than zero." << std::endl;
                return false;
            }
        }
        m_variables[symbol.m_name] = symbol;
        
        return true;
    }
    
    // error - duplicate symbol
    std::cerr << variable->getFilename() << ":" << variable->getLineNumber() << ":" << variable->getColumnNumber() << ": error: field \'" << variable->getIdentifier()->getIdentifier() << "\' of type " << IrTypeToString(variable->getType()) << " already declared in scope." << std::endl;
    
    return false;    
}
 
bool IrSymbolTable::addMethod(IrMethodDecl* method)
{
    bool ok = true;
    
    auto it = m_methods.find(method->getName());
    if (it == m_methods.end())
    {
        SMethodSymbol symbol;
        symbol.m_name = method->getName();
        symbol.m_type = method->getReturnType();
        SVariableSymbol argInfo;
        for (size_t i = 0; i < method->getNumArguments(); i++)
        {
            const IrVariableDecl* arg = method->getArgument(i);
            argInfo.m_name = arg->getVariable(0)->getIdentifier();
            argInfo.m_type = arg->getType();
            argInfo.m_count = 1;
            symbol.m_arguments.push_back(argInfo);
        }
        m_methods[symbol.m_name] = symbol;
    }
    else
    {
        // Error - duplicate
        std::cerr << method->getFilename() << ":" << method->getLineNumber() << ":" << method->getColumnNumber() << ": error: method \'" << method->getName() << "\' already declared in scope." << std::endl;                
        ok = false;
    }
    
    return ok;
}

bool IrSymbolTable::exists(IrLocation* variable) const
{ 
    auto it = m_variables.find(variable->getIdentifier()->getIdentifier());
    if (it != m_variables.end())
    {
        return true;
    }
    return false;     
}

bool IrSymbolTable::exists(IrMethodCall* method) const
{ 
    if (method->isExternal()) return false;
    
    bool exists = false;
    
    auto it = m_methods.find(method->getIdentifier()->getIdentifier());
    if (it != m_methods.end())
    {
        // match signature
        const SMethodSymbol& symbol = it->second;
        
        if (symbol.m_arguments.size() == method->getNumArguments())
        {
            exists = true;
            for (size_t i = 0; i < method->getNumArguments(); i++)
            {
                if (method->getArgumentType(i) != symbol.m_arguments.at(i).m_type)
                {
                    exists = false;
                    // Error incorrect argument type.
                    std::cerr << method->getFilename() << ":" << method->getLineNumber() << ":" << method->getColumnNumber() << ": error: method \'" << method->getIdentifier()->getIdentifier() << "\' incorrect argument type for argument, " 
                              << i << ".  Expected type " << IrTypeToString(symbol.m_arguments.at(i).m_type) << " but given " << IrTypeToString(method->getArgumentType(i)) << std::endl;                            
                    break;
                }
            }
        }
        else
        {
            // Error incorrect number of arguments.
            std::cerr << method->getFilename() << ":" << method->getLineNumber() << ":" << method->getColumnNumber() << ": error: method \'" << method->getIdentifier()->getIdentifier() << "\' incorrect number of arguments." << std::endl;                            
        }
    }
    else
    {
        // Error - not found
        //std::cerr << method->getFilename() << ":" << method->getLineNumber() << ":" << method->getColumnNumber() << ": error: method " << method->getIdentifier()->getIdentifier() << " is not defined." << std::endl;                
    }
    return exists; 
}

bool IrSymbolTable::getSymbol(IrLocation* variable, SVariableSymbol& symbol) const
{
    bool valid = false;
 
    auto it = m_variables.find(variable->getIdentifier()->getIdentifier());
    if (it != m_variables.end())
    {
        symbol = it->second;
        
        valid = true;
    }
    
    return valid;
}

bool IrSymbolTable::getSymbol(IrMethodCall* method, SMethodSymbol& symbol) const
{
    bool valid = false;
    
    if (exists(method))
    {
        auto it = m_methods.find(method->getIdentifier()->getIdentifier());
        if (it != m_methods.end())
        {
            symbol = it->second;
            valid = true;
        }
    }
    
    return valid;
}    

void IrSymbolTable::print(int depth)
{
    const size_t table_size = m_variables.size();
    
    if (table_size == 0) return;
    
    IRPRINT_INDENT(depth);
    std::cout << "Symbol Table Size = " << table_size << std::endl;
    
    IRPRINT_INDENT(depth);
    std::cout << "Name\t\tType\t\tCount" << std::endl;
    for (auto it = m_variables.cbegin(); it != m_variables.cend(); ++it)
    {
        IRPRINT_INDENT(depth);
        std::cout << it->first << "\t\t" << IrTypeToString(it->second.m_type) << "\t\t" << it->second.m_count << std::endl;
    }
}
 
} // namespace Decaf
